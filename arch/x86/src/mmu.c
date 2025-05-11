#pragma once

#include "arch/mmu.h"
#include "arch/asm.h"
#include "arch/bits.h"
#include "arch/isr.h"
#include "core/ffAllocator.h"
#include "core/kString.h"
#include "core/kernelHeap.h"
#include "core/memArea.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "isr.h"
#include "mmu.h"
#include "types.h"

static page_map_info_ct kernel_context;
static firstfit_pool_ct mmu_mem_pool;

static void page_fault_handler(isr_registers_ct *registers)
{
    vga_printf("pagefault, error code=%d\n", registers->error_code);
    while (1)
        ;
}

err_code_ct mmu_init()
{
    isr_set_handler(EXCVEC_PAGEFAULT, page_fault_handler);

    addr_ct mem_area = mem_area_alloc_with_alignment(MEM_AREA_SIZE, 4096);
    if (NULL == mem_area)
    {
        return NO_MEMORY;
    }

    firstfit_init(&mmu_mem_pool, kmalloc, kfree);
    bool_ct ret = firstfit_add_block(&mmu_mem_pool, mem_area, MEM_AREA_SIZE);

    if (!ret)
    {
        return NO_MEMORY;
    }

    kernel_context.pml4 = (pml4_entry_ct *)EARLY_PML4T_ADDRESS;
    kmemset(&kernel_context.pml4[1], 0, (PAGE_TABLE_NO_OF_ENTRIES - 2) * sizeof(table_entry_ct));
    kmemset(&((pdpt_entry_ct *)EARLY_PDPT_ADDRESS)[1], 0, (PAGE_TABLE_NO_OF_ENTRIES - 1) * sizeof(table_entry_ct));
    kmemset(&((pd_entry_ct *)EARLY_PAGE_DIRECTORY_ADDRESS)[2], 0,
            (PAGE_TABLE_NO_OF_ENTRIES - 2) * sizeof(table_entry_ct));

    return NO_ERROR;
}

static addr_ct alloc_table()
{
    table_entry_ct *table = (table_entry_ct *)firstfit_alloc_with_alignment(&mmu_mem_pool, PAGE_SIZE, PAGE_SIZE);
    if (NULL == table)
    {
        return NULL;
    }

    kmemset(table, 0, PAGE_SIZE);

    return table;
}

static inline void flush_tlb_entry(addr_ct addr)
{
    ASM("invlpg (%0)"
        :           /* no output operands */
        : "r"(addr) /* input operand is addr, passed in a register */
        : "memory"  /* tells compiler the instruction modifies memory */
    );
};

err_code_ct mmu_init_context(page_map_info_ct *info)
{
    info->pml4 = alloc_table();
}

page_map_info_ct *mmu_get_kernel_context()
{
    return &kernel_context;
}

static bool_ct virt_to_phys(page_map_info_ct *info, addr_ct virt_addr, addr_ct *physical_addr)
{
    u32_ct pml4_index = PML4_INDEX(virt_addr);
    u32_ct pdpt_index = PDPT_INDEX(virt_addr);
    u32_ct pd_index = PD_INDEX(virt_addr);
    u32_ct pt_index = PT_INDEX(virt_addr);
    u32_ct page_offset = PAGE_OFFSET(virt_addr);

    pdpt_entry_ct *pdpt;
    pd_entry_ct *pd;
    pt_entry_ct *pt;

    if ((info->pml4[pml4_index].present))
    {
        pdpt = (u64_ct *)(*(u64_ct *)&info->pml4[pml4_index] & PTE_ADDR_MASK);
        if (pdpt[pdpt_index].present)
        {
            pd = (u64_ct *)(*(u64_ct *)&pdpt[pdpt_index] & PTE_ADDR_MASK);
            if (pd[pd_index].present)
            {
                pt = (u64_ct *)(*(u64_ct *)&pd[pd_index] & PTE_ADDR_MASK);
                if (pt[pt_index].present)
                {
                    *physical_addr = (pt[pt_index].phy_address << PAGE_SHIFT) + page_offset;
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

err_code_ct mmu_map_memory(page_map_info_ct *info, addr_ct virt_addr, addr_ct physical_addr, u32_ct page_count,
                           u64_ct flags)
{
    if (PAGE_OFFSET(virt_addr) != 0 || PAGE_OFFSET(physical_addr) != 0)
    {
        return BAD_PARAM;
    }

    err_code_ct ret = NO_ERROR;

    u32_ct pml4_index;
    u32_ct pdpt_index;
    u32_ct pd_index;
    u32_ct pt_index;

    pml4_entry_ct *pml4_phys;
    if (!virt_to_phys(info, (addr_ct)info->pml4, (addr_ct *)&pml4_phys))
    {
        return BAD_MEMORY;
    }

    pdpt_entry_ct *pdpt, *pdpt_phys;
    pd_entry_ct *pd, *pd_phys;
    pt_entry_ct *pt, *pt_phys;

    bool_ct allocated_pdpt = FALSE;
    bool_ct allocated_pd = FALSE;
    bool_ct allocated_pt = FALSE;

    for (u32_ct i = 0; i < page_count; i++)
    {
        pml4_index = PML4_INDEX(virt_addr);
        pdpt_index = PDPT_INDEX(virt_addr);
        pd_index = PD_INDEX(virt_addr);
        pt_index = PT_INDEX(virt_addr);

        if (!(info->pml4[pml4_index].present))
        {
            pdpt = alloc_table();
            if (NULL == pdpt)
            {
                ret = NO_MEMORY;
                break;
            }
            allocated_pdpt = TRUE;

            if (!virt_to_phys(info, pdpt, &pdpt_phys))
            {
                ret = BAD_MEMORY;
                break;
            }

            *((u64_ct *)&info->pml4[pml4_index]) =
                (((u64_ct)pdpt_phys & PTE_ADDR_MASK) | PTE_FLAG_PRESENT | PTE_FLAG_READ_WRITE | flags);
        }
        else
        {
            pdpt = (u64_ct *)(*(u64_ct *)&info->pml4[pml4_index] & PTE_ADDR_MASK);
        }

        if (!(pdpt[pdpt_index].present))
        {
            pd = alloc_table();
            if (NULL == pd)
            {
                ret = NO_MEMORY;
                break;
            }
            allocated_pd = TRUE;

            if (!virt_to_phys(info, pd, &pd_phys))
            {
                ret = BAD_MEMORY;
                break;
            }

            *((u64_ct *)&pdpt[pdpt_index]) =
                (((u64_ct)pd_phys & PTE_ADDR_MASK) | PTE_FLAG_PRESENT | PTE_FLAG_READ_WRITE | flags);
        }
        else
        {
            pd = (u64_ct *)(*(u64_ct *)&pdpt[pdpt_index] & PTE_ADDR_MASK);
        }

        if (!(pd[pd_index].present))
        {
            pt = alloc_table();
            if (NULL == pt)
            {
                ret = NO_MEMORY;
                break;
            }
            allocated_pt = TRUE;

            if (!virt_to_phys(info, pt, &pt_phys))
            {
                ret = BAD_MEMORY;
                break;
            }

            *((u64_ct *)&pd[pd_index]) =
                (((u64_ct)pt_phys & PTE_ADDR_MASK) | PTE_FLAG_PRESENT | PTE_FLAG_READ_WRITE | flags);
        }
        else
        {
            pt = (u64_ct *)(*(u64_ct *)&pd[pd_index] & PTE_ADDR_MASK);
        }

        *((u64_ct *)&pt[pt_index]) =
            (((u64_ct)physical_addr & PTE_ADDR_MASK) | PTE_FLAG_PRESENT | PTE_FLAG_READ_WRITE | flags);

        flush_tlb_entry(virt_addr);

        virt_addr += PAGE_SIZE;
        physical_addr += PAGE_SIZE;
    }

    if (NO_ERROR != ret)
    {
        bool_ct free_ret = TRUE;
        if (allocated_pdpt)
        {
            free_ret = firstfit_free(&mmu_mem_pool, pdpt);
        }
        if (allocated_pd)
        {
            free_ret &= firstfit_free(&mmu_mem_pool, pd);
        }
        if (allocated_pt)
        {
            free_ret &= firstfit_free(&mmu_mem_pool, pt);
        }
        if (!free_ret)
        {
            /* Pool corrupted! */
        }
    }

    return ret;
}
