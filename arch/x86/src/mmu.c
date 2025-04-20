#pragma once
/*
#include "mmu.h"
#include "arch/bits.h"
#include "ffAllocator.h"
#include "kString.h"
#include "types.h"
*/
// 512 GB
/*static mmu_table_ct early_pml4t = (pml4_entry_ct *)EARLY_PML4T_ADDRESS;

// 1 GB
static mmu_table_ct early_pdpt = (pdpt_entry_ct *)EARLY_PDPT_ADDRESS;

// 2 MB
static mmu_table_ct early_pd = (pd_entry_ct *)EARLY_PAGE_DIRECTORY_ADDRESS;

// 4 KB
static mmu_table_ct early_pt = (pt_entry_ct)EARLY_PAGE_TABLE_1_ADDRESS;

static void init_table(mmu_table_ct *table, addr_ct start_addr)
{
    table->first_entry = start_addr;
    table->no_of_entries = 0;
    kmemset(&table->alloc_bitmap, 1, PAGE_TABLE_NO_OF_ENTRIES / 8);
}

static inline addr_ct alloc_entry_from_table(mmu_table_ct *table)
{
    arch_get_msb() if (NULL == entry)
    {
        return NULL;
    }

    table->no_of_entries++;

    return entry;
}

bool_ct mmu_init()
{

    early_pdpt.first_entry = EARLY_PDPT_ADDRESS;
    early_pdpt.no_of_entries = 1;

    early_pdpt.first_entry = EARLY_PDPT_ADDRESS;
    early_pdpt.no_of_entries = 1;
}

void mmu_map_memory(addr_ct virt_addr, addr_ct physical_addr)
{
}
*/