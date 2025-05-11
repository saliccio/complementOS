#pragma once

#include "arch/mmu.h"
#include "core/ffAllocator.h"
#include "errCodes.h"

#define KERNEL_VIRT_ADDR_SPACE_START 0xFFFFFF8000000000
#define KERNELIZED_ADDR(addr) (KERNEL_VIRT_ADDR_SPACE_START + addr)

typedef struct virt_mem_info
{
    firstfit_pool_ct virt_addr_space;
    page_map_info_ct *context;
} virt_mem_info_ct;

typedef enum map_flags
{
    READ_WRITE,
    USER
} map_flags_ct;

virt_mem_info_ct *mem_get_kernel_mem_info();
err_code_ct mem_map_to_phys_addr(virt_mem_info_ct *mem_info, addr_ct virt_addr, addr_ct physical_addr,
                                 size_ct page_count, map_flags_ct flags);
err_code_ct mem_map(virt_mem_info_ct *mem_info, addr_ct virt_addr, size_ct page_count, map_flags_ct flags);