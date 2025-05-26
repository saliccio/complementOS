#include "core/memArea.h"
#include "core/ffAllocator.h"
#include "core/kernelHeap.h"
#include "core/ld.h"

SECTION(".data") static firstfit_pool_ct mem_area_pool;

bool_ct mem_area_init()
{
    firstfit_init(&mem_area_pool, kmalloc, kfree);
    bool_ct ret =
        firstfit_add_block(&mem_area_pool, &_kernel_area_start, (size_ct)(&_kernel_area_end - &_kernel_area_start));
    if (!ret)
    {
        return FALSE;
    }

    return TRUE;
}

addr_ct mem_area_alloc(size_ct size)
{
    return firstfit_alloc(&mem_area_pool, size);
}

addr_ct mem_area_alloc_with_start_addr(size_ct size, addr_ct start_addr)
{
    return firstfit_alloc_with_start_addr(&mem_area_pool, size, start_addr);
}

addr_ct mem_area_alloc_with_alignment(size_ct size, size_ct alignment)
{
    return firstfit_alloc_with_alignment(&mem_area_pool, size, alignment);
}