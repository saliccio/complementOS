#pragma once

#include "arch/mmu.h"
#include "core/ffAllocator.h"
#include "errCodes.h"

#define KERNEL_VIRT_ADDR_SPACE_START 0xFFFFFF8000000000
#define KERNELIZED_ADDR(addr) (KERNEL_VIRT_ADDR_SPACE_START + (addr_ct)addr)

typedef struct virt_mem_info
{
    firstfit_pool_ct virt_addr_space;
    page_map_info_ct *context;
} virt_mem_info_ct;

typedef enum map_flags
{
    PTE_READ_WRITE = 0x1,
    PTE_USER = 0x2,
    PTE_WRITE_THROUGH = 0x4,
    PTE_UNCACHEABLE = 0x8,
    PTE_EXEC_DISABLED = 0x10,
    FORCE_MAP = 0x20
} map_flags_ct;

virt_mem_info_ct *mem_get_kernel_mem_info();
err_code_ct mem_map_virt_addr(virt_mem_info_ct *mem_info, addr_ct virt_addr, addr_ct physical_addr, size_ct page_count,
                              map_flags_ct flags);
err_code_ct mem_map(virt_mem_info_ct *mem_info, addr_ct *virt_addr, addr_ct physical_addr, size_ct page_count,
                    map_flags_ct flags);