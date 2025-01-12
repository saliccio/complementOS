#include "paging.h"
#include <drivers/d_screen.h>

static u32_ct next_available_indexes[PAGE_DIRECTORY_SIZE - 1];  // Next available entry addresses for each page table
static u32_ct page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096)));
static u32_ct first_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(4096)));

void paging_init() {
    for (u32_ct i = 0; i < 1023; i++) {
        next_available_indexes[i] = 0;
        u32_ct page_table_address = (u32_ct)(first_page_table + i);
        page_directory[i] = PAGE_DIRECTORY_ENTRY(page_table_address);
    }
    page_directory[1023] = PAGE_DIRECTORY_ENTRY((u32_ct)page_directory);  // Apply recursive paging

    for (u32_ct i = 0; i < 1024; i++) {  // Apply identity paging for the first 4 MB
        first_page_table[i] = PAGE_TABLE_ENTRY(i);
    }

    for (u32_ct i = 1; i < PAGE_DIRECTORY_SIZE; i++) {
        for (u32_ct j = 0; j < PAGE_TABLE_SIZE; j++) {
            *(&first_page_table[j] + i * PAGE_TABLE_SIZE) = PAGE_TABLE_DEFAULT_ENTRY;
        }
    }

    __asm__("mov %0, %%cr3" : : "a" (page_directory));  // Move page directory address into control register 3
    __asm__("mov %cr0, %eax");  // Move control register 0 into eax to modify
    __asm__("or $0x80000000, %eax");  // Set 32th bit of eax
    __asm__("mov %eax, %cr0");  // Move eax into control register 0 (enables paging)
}

/*void* allocate_pages(u32_ct page_table_index, u32_ct no_of_pages, u32_ct physical_address) {
    if (page_table_index > 1022) return NULL;

    u32_ct* page_table_starting_address = (u32_ct*) VIRTUAL_ADDRESS_OF_PAGE_TABLE(page_table_index);
    for (u32_ct i = 0; i < no_of_pages; i++) {
        u32_ct* next_available_entry_ptr = page_table_starting_address + next_available_indexes[page_table_index];
        while (*next)

        *next_available_entry_ptr = PAGE_TABLE_ENTRY(physical_address);
        next_available_indexes[page_table_index] = (next_available_indexes[page_table_index] + 1) % PAGE_TABLE_SIZE;
    }
}*/