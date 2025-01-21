/*
#include "paging.h"
#include "archAsm.h"

#define TABLE_SECTION_ATTRIBUTE __attribute__((section(".init_tables")))

static u32_ct next_available_indexes[PAGE_DIRECTORY_SIZE - 1]; // Next available entry addresses for
                                                               // each page table
static u32_ct page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_SIZE)));
static u32_ct kernel_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

static u64_ct pml4t[PML4T_SIZE] TABLE_SECTION_ATTRIBUTE;                     // Page-map level 4 table
static u64_ct first_pdpt[PDPT_SIZE] TABLE_SECTION_ATTRIBUTE;                 // Page directory pointer table
static u64_ct first_page_directory_table[PDPT_SIZE] TABLE_SECTION_ATTRIBUTE; // Page directory table
static u64_ct first_page_table[PAGE_TABLE_SIZE] TABLE_SECTION_ATTRIBUTE;     // Page table for the first megabyte

void paging_init()
{
    pml4t[0] = (u64_ct)first_pdpt | PAGE_FLAG_READ_WRITE | PAGE_FLAG_PRESENT;
    first_pdpt[0] = (u64_ct)first_page_directory_table | PAGE_FLAG_READ_WRITE | PAGE_FLAG_PRESENT;
    first_page_directory_table[0] = (u64_ct)first_page_table | PAGE_FLAG_READ_WRITE | PAGE_FLAG_PRESENT;

    for (u32_ct i = 0; i < PAGE_TABLE_SIZE; i++)
    { // Apply identity paging for the first table (2 MB)
        first_page_table[i] = ((i * PAGE_SIZE) | PAGE_FLAG_READ_WRITE | PAGE_FLAG_PRESENT);
    }

    // Enable PAE (physical address extension)
    ASM("mov %cr4, %edx");
    ASM("or $(1 << 5), %edx");
    ASM("mov %edx, %cr4");

    // Move pml4t address into control register 3
    ASM("mov %0, %%cr3" : : "a"(pml4t));
}

void *allocate_pages(u32_ct page_table_index, u32_ct no_of_pages, u32_ct physical_address)
{
    if (page_table_index > 1022)
        return NULL;

    u32_ct *page_table_starting_address = (u32_ct *)VIRTUAL_ADDRESS_OF_PAGE_TABLE(page_table_index);
    for (u32_ct i = 0; i < no_of_pages; i++)
    {
        u32_ct *next_available_entry_ptr = page_table_starting_address + next_available_indexes[page_table_index];
        while (*next)

            *next_available_entry_ptr = PAGE_TABLE_ENTRY(physical_address);
        next_available_indexes[page_table_index] = (next_available_indexes[page_table_index] + 1) % PAGE_TABLE_SIZE;
    }
}

*/