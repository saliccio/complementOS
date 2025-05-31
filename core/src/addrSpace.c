#include "addrSpace.h"
#include "arch/bootInfo.h"
#include "arch/mmu.h"
#include "core/addrSpace.h"
#include "core/kernelHeap.h"
#include "core/ld.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"

SECTION(".data") static virt_mem_info_ct kernel_mem_info;

err_code_ct mem_init_kernel_addr_space()
{
    firstfit_init(&kernel_mem_info.virt_addr_space, kmalloc, kfree);
    bool_ct ret = firstfit_add_block(&kernel_mem_info.virt_addr_space, (addr_ct)KERNEL_VIRT_ADDR_SPACE_START,
                                     (~(size_ct)0) - KERNEL_VIRT_ADDR_SPACE_START);

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
                                    size_ct page_count, map_flags_ct flags, bool_ct alloc_virt_addr)
{
    err_code_ct ret = NO_ERROR;
    size_ct size = page_count * PAGE_SIZE;
    addr_ct ret_virt_addr = NULL;
    bool_ct force_map = flags & FORCE_MAP;

    if (alloc_virt_addr && !force_map)
    {
        addr_ct first_virt_addr = firstfit_get_first_free_address(&mem_info->virt_addr_space);
        ret_virt_addr = firstfit_alloc_with_start_addr(&mem_info->virt_addr_space, size, virt_addr);
        if (NULL == ret_virt_addr && NULL != first_virt_addr)
        {
            ret = NO_MEMORY;
        }
    }

    if (!force_map)
    {
    }

    if (NO_ERROR == ret)
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

        if (NO_ERROR == ret)
        {
            if (alloc_virt_addr && force_map)
            {
                /* Lazily allocate virtual address space: */
                firstfit_force_alloc(&mem_info->virt_addr_space, size, virt_addr);
            }
        }
        else
        {
            if (NULL != ret_virt_addr)
            {
                bool_ct free_ret = firstfit_free(&mem_info->virt_addr_space, virt_addr, size);
                if (!free_ret)
                {
                    /* Pool corrupted! */
                }
            }
        }
    }

    return ret;
}

err_code_ct mem_map_virt_addr(virt_mem_info_ct *mem_info, addr_ct virt_addr, addr_ct physical_addr, size_ct page_count,
                              map_flags_ct flags)
{
    if (PAGE_OFFSET(virt_addr) != 0 || PAGE_OFFSET(physical_addr) != 0 || page_count == 0)
    {
        return BAD_PARAM;
    }

    return map_inner(mem_info, virt_addr, physical_addr, page_count, flags, TRUE);
}

err_code_ct mem_map(virt_mem_info_ct *mem_info, addr_ct *virt_addr, addr_ct physical_addr, size_ct page_count,
                    map_flags_ct flags)
{
    if (NULL == virt_addr || PAGE_OFFSET(physical_addr) != 0 || page_count == 0)
    {
        return BAD_PARAM;
    }

    size_ct size = page_count * PAGE_SIZE;

    addr_ct first_virt_addr = firstfit_get_first_free_address(&mem_info->virt_addr_space);
    *virt_addr = firstfit_alloc_with_alignment(&mem_info->virt_addr_space, size, PAGE_SIZE);
    if (NULL == *virt_addr && NULL != first_virt_addr)
    {
        return NO_MEMORY;
    }

    err_code_ct ret = map_inner(mem_info, *virt_addr, physical_addr, page_count, flags, FALSE);

    return ret;
}

extern addr_ct _kernel_area_paddr_start;
extern addr_ct _kernel_area_paddr_end;

/** PRECONDITIONS:
 *  - Physical page pool must have no allocations after its initialization.
 *  - Both &_kernel_area_start and &_kernel_area_end must be page-aligned.
 */
static err_code_ct make_kernel_mappings()
{
    err_code_ct ret = NO_ERROR;

    /* These mappings are made in the boot stage: */
    ret = mem_map_virt_addr(&kernel_mem_info, (addr_ct)KERNEL_VIRT_ADDR_SPACE_START, (addr_ct)0, IDENTITY_MAPPED_PAGES,
                            PTE_READ_WRITE | FORCE_MAP);

    if (NO_ERROR == ret)
    {
        /* Map the entire kernel area: */
        ret = mem_map_virt_addr(&kernel_mem_info, &_kernel_area_start, &_kernel_area_paddr_start,
                                ((size_ct)&_kernel_area_end - (size_ct)&_kernel_area_start) / PAGE_SIZE,
                                PTE_READ_WRITE | FORCE_MAP);
    }

    return ret;
}

ATTACH_STATIC_HOOK(MMU_INIT_END, make_kernel_mappings, 99);