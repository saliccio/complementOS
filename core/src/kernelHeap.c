#include "core/kernelHeap.h"
#include "buddyAllocator.h"

static buddy_pool_ct kheap_pool;

bool_ct kheap_init(addr_ct start)
{
    return buddy_init(&kheap_pool, start);
}

addr_ct kmalloc(size_ct size)
{
    return buddy_alloc(&kheap_pool, size);
}

void kfree(addr_ct addr)
{
    buddy_free(&kheap_pool, addr);
}

size_ct kheap_get_free_space()
{
    return buddy_get_free_space(&kheap_pool);
}