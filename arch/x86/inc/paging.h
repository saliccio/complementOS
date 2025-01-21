#pragma once

#include <types.h>

#define PAGE_SIZE 4096

#define PML4T_SIZE 512
#define PDPT_SIZE 512
#define PAGE_DIRECTORY_SIZE 512
#define PAGE_TABLE_SIZE 512

#define PAGE_FLAG_PRESENT 1
#define PAGE_FLAG_READ_WRITE 2

#define PAGE_DIRECTORY_DEFAULT_ENTRY 0x00000002

// Flags: Supervisor, read/write, present
#define PAGE_TABLE_ENTRY(frame_index) ((frame_index * 0x1000) | 3)

#define VIRTUAL_ADDRESS_OF_PAGE_TABLE(index) (0xFFC00000 + index * 0x1000)

void paging_init();

// Allocates given number of pages at given page table index pointing to the given physical address. Returns the
// corresponding virtual address.
void *allocate_pages(u32_ct page_table_index, u32_ct no_of_pages, void *physical_address);