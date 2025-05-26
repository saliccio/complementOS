#pragma once

#include "errCodes.h"
#include "types.h"

typedef struct page_map_info page_map_info_ct;

#define PTE_FLAG_PRESENT (1 << 0)
#define PTE_FLAG_READ_WRITE (1 << 1)
#define PTE_FLAG_USER (1 << 2)
#define PTE_FLAG_PWT (1 << 3)
#define PTE_FLAG_PCD (1 << 4)
#define PTE_FLAG_ACCESSED (1 << 5)
#define PTE_FLAG_XD (1 << 63)
#define PTE_ADDR_MASK 0x000FFFFFFFFFF000ULL

#define PAGE_SIZE 4096

// Extract page offset (bits 0-11)
#define PAGE_OFFSET(addr) ((u64_ct)(addr)&0xFFF)

err_code_ct mmu_init();
page_map_info_ct *mmu_get_kernel_context();
err_code_ct mmu_map_memory(page_map_info_ct *info, addr_ct virt_addr, addr_ct physical_addr, u32_ct page_count,
                           u64_ct flags);