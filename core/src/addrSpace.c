#include "addrSpace.h"
#include "arch/mmu.h"
#include "core/addrSpace.h"
#include "core/kernelHeap.h"
#include "drivers/d_screen.h"

static virt_mem_info_ct kernel_mem_info;
static firstfit_pool_ct physical_addr_space;

err_code_ct mem_init_kernel_addr_space()
{
    firstfit_init(&kernel_mem_info.virt_addr_space, kmalloc, kfree);
    bool_ct ret = firstfit_add_block(&kernel_mem_info.virt_addr_space, (addr_ct)KERNEL_VIRT_ADDR_SPACE_START,
                                     ~0 - KERNEL_VIRT_ADDR_SPACE_START);

    firstfit_init(&physical_addr_space, kmalloc, kfree);
    ret |= firstfit_add_block(&physical_addr_space, (addr_ct)0, ~0);
    (void)firstfit_alloc_with_start_addr(&physical_addr_space, 0, (addr_ct)0x400000);

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
    addr_ct ret_phys_addr = firstfit_alloc_with_start_addr(&physical_addr_space, size, physical_addr);
    if (NULL == ret_phys_addr)
    {
        return NO_MEMORY;
    }

    addr_ct ret_virt_addr = firstfit_alloc_with_start_addr(&mem_info->virt_addr_space, size, virt_addr);
    err_code_ct ret = NO_ERROR;
    if (NULL == ret_virt_addr)
    {
        ret = NO_MEMORY;
    }
    else
    {
        u32_ct mmu_flags = 0;
        if ((flags & READ_WRITE))
        {
            mmu_flags |= PTE_FLAG_READ_WRITE;
        }
        if (flags & USER)
        {
            mmu_flags |= PTE_FLAG_USER;
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
    if (PAGE_OFFSET(virt_addr) != 0 || PAGE_OFFSET(physical_addr) != 0)
    {
        return BAD_PARAM;
    }

    return map_inner(mem_info, virt_addr, physical_addr, page_count, flags);
}

err_code_ct mem_map(virt_mem_info_ct *mem_info, addr_ct virt_addr, size_ct page_count, map_flags_ct flags)
{
    if (PAGE_OFFSET(virt_addr) != 0)
    {
        return BAD_PARAM;
    }

    size_ct size = page_count * PAGE_SIZE;
    addr_ct physical_addr = firstfit_alloc_with_alignment(&physical_addr_space, size, PAGE_SIZE);
    if (NULL == physical_addr)
    {
        return NO_MEMORY;
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