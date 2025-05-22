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

addr_ct kmalloc_without_header(size_ct size)
{
    addr_ct addr = kmalloc(size - sizeof(buddy_block_header_ct));
    if (NULL == addr)
    {
        return addr;
    }

    return addr - sizeof(buddy_block_header_ct);
}

void kfree(addr_ct addr)
{
    buddy_free(&kheap_pool, addr);
}

void kfree_without_header(addr_ct addr, size_ct size)
{
    *(buddy_block_header_ct *)addr = size | BUDDY_IN_USE_FLAG;
    kfree(addr + sizeof(buddy_block_header_ct));
}

size_ct kheap_get_free_space()
{
    return buddy_get_free_space(&kheap_pool);
}