#include "buddyAllocator.h"
#include "arch/bits.h"
#include "drivers/d_screen.h"
#include "kString.h"

#define ORDER_TO_SIZE(order) (1 << (order + BUDDY_FIRST_ORDER_OFFSET))
#define IS_ALIGNED(addr, alignment) (!((addr) & ((alignment)-1)))
#define ALIGN_UP_TO(size, alignment) (((size) + (alignment)-1) & (~((alignment)-1)))
#define POINTER_TO_ULONG(addr) (*(ulong_ct *)(&addr))
#define IN_USE_FLAG (1)

static inline u32_ct size_to_order(size_ct size)
{
    u32_ct order = 0;
    size_ct block_size = BUDDY_MIN_BLOCK_SIZE;
    while (block_size < size)
    {
        block_size <<= 1;
        order++;
    }

    return order;
}

bool_ct buddy_init(buddy_pool_ct *pool, addr_ct start)
{
    if (!IS_ALIGNED(POINTER_TO_ULONG(start), BUDDY_MAX_BLOCK_SIZE))
    {
        return FALSE;
    }

    kmemset((u8_ct *)pool, 0, sizeof(buddy_pool_ct));
    pool->pool_start = start;
    pool->free_lists[BUDDY_MAX_ORDER] = start;
    pool->free_lists[BUDDY_MAX_ORDER]->prev_free_block = NULL;
    pool->free_lists[BUDDY_MAX_ORDER]->next_free_block = NULL;

    return TRUE;
}

static void split_block_at_order(buddy_pool_ct *pool, u32_ct order)
{
    if (order <= 0 || order > BUDDY_MAX_ORDER || NULL == pool->free_lists[order])
    {
        return;
    }

    buddy_block_ct *block = pool->free_lists[order];
    pool->free_lists[order] = block->next_free_block;
    if (NULL != pool->free_lists[order])
    {
        pool->free_lists[order]->prev_free_block = NULL;
    }

    u32_ct half_size = ORDER_TO_SIZE(order - 1);
    buddy_block_ct *buddy = (buddy_block_ct *)((addr_ct)block + half_size);
    buddy_block_ct *list_head = pool->free_lists[order - 1];
    if (NULL != list_head)
    {
        list_head->prev_free_block = buddy;
    }
    block->next_free_block = buddy;
    buddy->prev_free_block = block;
    buddy->next_free_block = list_head;
    pool->free_lists[order - 1] = block;
}

addr_ct buddy_alloc(buddy_pool_ct *pool, size_ct size)
{
    if (0 == size)
    {
        return NULL;
    }

    u32_ct actual_size = size + sizeof(buddy_block_header_ct);
    u32_ct order = size_to_order(actual_size);
    if (order > BUDDY_MAX_ORDER)
    {
        return NULL;
    }

    u32_ct i = order;
    while (i <= BUDDY_MAX_ORDER && NULL == pool->free_lists[i])
    {
        i++;
    }

    if (i > BUDDY_MAX_ORDER)
    {
        return NULL;
    }

    while (i > order)
    {
        split_block_at_order(pool, i);
        i--;
    }

    buddy_block_ct *allocated_block = pool->free_lists[order];
    size_ct allocated_size = ORDER_TO_SIZE(order);
    allocated_block->size = allocated_size | IN_USE_FLAG;
    pool->free_lists[order] = allocated_block->next_free_block;
    if (NULL != pool->free_lists[order])
    {
        pool->free_lists[order]->prev_free_block = NULL;
    }
    pool->total_used += allocated_size;

    return ((addr_ct)allocated_block + sizeof(buddy_block_header_ct));
}

static void merge_blocks(buddy_pool_ct *pool, buddy_block_ct *block, u32_ct order)
{
    if (order > BUDDY_MAX_ORDER)
    {
        return;
    }

    if (order == BUDDY_MAX_ORDER)
    {
        block->prev_free_block = NULL;
        block->next_free_block = NULL;
        pool->free_lists[order] = block;
        return;
    }

    addr_ct block_addr = block;
    buddy_block_ct *buddy_block = (buddy_block_ct *)(POINTER_TO_ULONG(block_addr) ^ block->size);
    if (buddy_block->size & IN_USE_FLAG)
    {
        buddy_block_ct *list_head = pool->free_lists[order];
        block->prev_free_block = NULL;
        block->next_free_block = list_head;
        list_head->prev_free_block = block;
        pool->free_lists[order] = block;
    }
    else
    {
        // Remove buddy from the list:
        if (NULL == buddy_block->prev_free_block) // If at the head of the list
        {
            pool->free_lists[order] = buddy_block->next_free_block;
        }
        else
        {
            buddy_block->prev_free_block->next_free_block = buddy_block->next_free_block;
        }

        if (NULL != buddy_block->next_free_block)
        {
            buddy_block->next_free_block->prev_free_block = buddy_block->prev_free_block;
        }

        // Merge upwards recursively:
        buddy_block_ct *block_with_lesser_addr = (block < buddy_block ? block : buddy_block);
        merge_blocks(pool, block_with_lesser_addr, order + 1);
    }
}

void buddy_free(buddy_pool_ct *pool, addr_ct addr)
{
    if (NULL == addr)
    {
        return;
    }

    buddy_block_ct *block = (buddy_block_ct *)(addr - sizeof(buddy_block_header_ct));

    if (!(block->size & IN_USE_FLAG)) // Check if it is a double-free
    {
        return;
    }

    block->size &= ~IN_USE_FLAG;
    pool->total_used -= block->size;
    u32_ct order = size_to_order(block->size);
    merge_blocks(pool, block, order);
}

size_ct buddy_get_free_space(buddy_pool_ct *pool)
{
    return BUDDY_MAX_BLOCK_SIZE - pool->total_used;
}