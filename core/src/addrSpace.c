#include "addrSpace.h"
#include "arch/mmu.h"
#include "core/addrSpace.h"
#include "core/kernelHeap.h"
#include "core/ld.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"

SECTION(".data") static virt_mem_info_ct kernel_mem_info;
SECTION(".data") static firstfit_pool_ct physical_addr_space;

err_code_ct mem_init_kernel_addr_space()
{
    firstfit_init(&kernel_mem_info.virt_addr_space, kmalloc, kfree);
    bool_ct ret = firstfit_add_block(&kernel_mem_info.virt_addr_space, (addr_ct)KERNEL_VIRT_ADDR_SPACE_START,
                                     ~(size_ct)0 - KERNEL_VIRT_ADDR_SPACE_START);

    firstfit_init(&physical_addr_space, kmalloc, kfree);
    ret |= firstfit_add_block(&physical_addr_space, (addr_ct)0, ~(size_ct)0);

    if (!ret)
    {
        return NO_MEMORY;
    }

    kernel_mem_info.context = mmu_get_kernel_context();

    return NO_ERROR;
}

virt_mem_info_ct *mem_get_kernel_mem_info()
{
    return &kernel_mem_info;
}

static inline err_code_ct map_inner(virt_mem_info_ct *mem_info, addr_ct virt_addr, addr_ct physical_addr,
                                    size_ct page_count, map_flags_ct flags)
{
    size_ct size = page_count * PAGE_SIZE;

    addr_ct ret_virt_addr = firstfit_alloc_with_start_addr(&mem_info->virt_addr_space, size, virt_addr);
    err_code_ct ret = NO_ERROR;
    if (NULL == ret_virt_addr && !(flags & FORCE_IF_ALREADY_MAPPED))
    {
        ret = ALREADY_MAPPED;
    }
    else
    {
        u32_ct mmu_flags = 0;
        if ((flags & PTE_READ_WRITE))
        {
            mmu_flags |= PTE_FLAG_READ_WRITE;
        }
        if (flags & PTE_USER)
        {
            mmu_flags |= PTE_FLAG_USER;
        }
        if (flags & PTE_WRITE_THROUGH)
        {
            mmu_flags |= PTE_FLAG_PWT;
        }
        if (flags & PTE_UNCACHEABLE)
        {
            mmu_flags |= PTE_FLAG_PCD;
        }
        if (flags & PTE_EXEC_DISABLED)
        {
            mmu_flags |= PTE_FLAG_XD;
        }
        ret = mmu_map_memory(mem_info->context, virt_addr, physical_addr, page_count, mmu_flags);
    }

    if (NO_ERROR != ret)
    {
        bool_ct free_ret = firstfit_free(&physical_addr_space, physical_addr);
        if (NULL != ret_virt_addr)
        {
            free_ret &= firstfit_free(&mem_info->virt_addr_space, virt_addr);
        }
        if (!free_ret)
        {
            /* Pool corrupted! */
        }
    }

    return ret;
}

err_code_ct mem_map_to_phys_addr(virt_mem_info_ct *mem_info, addr_ct virt_addr, addr_ct physical_addr,
                                 size_ct page_count, map_flags_ct flags)
{
    if (PAGE_OFFSET(virt_addr) != 0 || PAGE_OFFSET(physical_addr) != 0 || page_count == 0)
    {
        return BAD_PARAM;
    }

    return map_inner(mem_info, virt_addr, physical_addr, page_count, flags);
}

err_code_ct mem_map(virt_mem_info_ct *mem_info, addr_ct virt_addr, size_ct page_count, map_flags_ct flags)
{
    if (PAGE_OFFSET(virt_addr) != 0 || page_count == 0)
    {
        return BAD_PARAM;
    }

    size_ct size = page_count * PAGE_SIZE;
    addr_ct physical_addr = firstfit_alloc_with_alignment(&physical_addr_space, size, PAGE_SIZE);
    if (!(flags & FORCE_IF_ALREADY_MAPPED) &&
        (NULL != firstfit_get_first_free_address(&physical_addr_space) && NULL == physical_addr))
    {
        return ALREADY_MAPPED;
    }
    else
    {
        err_code_ct ret = map_inner(mem_info, virt_addr, physical_addr, page_count, flags);
        if (NO_ERROR != ret)
        {
            bool_ct free_ret = firstfit_free(&physical_addr_space, physical_addr);
            if (!free_ret)
            {
                /* Pool corrupted! */
            }

            return ret;
        }
    }

    return NO_ERROR;
}

/** PRECONDITIONS:
 *  - Physical page pool must have no allocations after its initialization.
 *  - Both &_kernel_area_start and &_kernel_area_end must be page-aligned.
 */
static err_code_ct make_kernel_mappings()
{
    err_code_ct ret = NO_ERROR;

    /* These mappings are made in the boot stage: */
    (void)firstfit_alloc_with_start_addr(&physical_addr_space, 1024 * PAGE_SIZE, (addr_ct)0);
    if ((addr_ct)0 == firstfit_get_first_free_address(&physical_addr_space))
    {
        ret = BAD_MEMORY;
    }
    else
    {
        mem_map_to_phys_addr(&kernel_mem_info, (addr_ct)KERNEL_VIRT_ADDR_SPACE_START, (addr_ct)0, 1024, PTE_READ_WRITE);

        if (NO_ERROR == ret)
        {
            addr_ct first_free_virt_addr = firstfit_get_first_free_address(&kernel_mem_info.virt_addr_space);
            size_ct unmapped_kernel_area_right_offset = (size_ct)&_kernel_area_end - (size_ct)first_free_virt_addr;

            /* If not overflowed (> 0) */
            if (unmapped_kernel_area_right_offset > 0 && unmapped_kernel_area_right_offset < (size_ct)&_kernel_area_end)
            {
                ret = mem_map(&kernel_mem_info, first_free_virt_addr, unmapped_kernel_area_right_offset / PAGE_SIZE,
                              PTE_READ_WRITE);
            }
        }
    }

    return ret;
}

// ATTACH_STATIC_HOOK(MMU_INIT_END, make_kernel_mappings, 90);