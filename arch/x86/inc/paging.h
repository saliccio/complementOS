#pragma once

#include <types.h>

void paging_init();

// Allocates given number of pages at given page table index pointing to the given physical address. Returns the
// corresponding virtual address.
void *allocate_pages(u32_ct page_table_index, u32_ct no_of_pages, void *physical_address);