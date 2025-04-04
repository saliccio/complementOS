#include "kernelHeap.h"
#include "buddyAllocator.h"

static buddy_pool_ct kheap_pool;

bool_ct kheap_init()
{
    return buddy_init(&kheap_pool, &_kheap_start);
}

addr_ct kmalloc(size_ct size)
{
    return buddy_alloc(&kheap_pool, size);
}

void kfree(addr_ct addr)
{
    buddy_free(&kheap_pool, addr);
}