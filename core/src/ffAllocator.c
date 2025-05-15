#include "core/ffAllocator.h"
#include "core/kernelHeap.h"
#include "drivers/d_screen.h"

#define PTR_TO_SIZE(pointer) ((size_ct)(pointer))

#define IS_ADDR_ALIGNED(addr, alignment) (0 == (PTR_TO_SIZE(addr) & ((alignment)-1)))

#define ALIGN_UP_TO(addr, alignment) ((addr_ct)((PTR_TO_SIZE(addr) + (alignment)-1) & ~((alignment)-1)))

#define IS_BETWEEN(value, min, max) ((value) > (min) && (value) < (max))

typedef enum ffit_alloc_constraint
{
    NO_CONSTRAINTS = 0, // Except size :)
    START_ADDRESS = 1,
    ALIGNMENT = 2,
} ffit_alloc_constraint_ct;

void firstfit_init(firstfit_pool_ct *pool, addr_ct (*xmalloc)(size_ct size), void (*xfree)(addr_ct addr))
{
    pool->first_block = NULL;
    pool->first_alloc_info = NULL;
    pool->xmalloc = xmalloc;
    pool->xfree = xfree;
}

static inline bool_ct create_block(firstfit_pool_ct *pool, firstfit_block_ct **block, firstfit_block_ct *next,
                                   size_ct size, addr_ct start)
{
    *block = pool->xmalloc(sizeof(firstfit_block_ct));
    if (NULL == *block)
    {
        return FALSE;
    }

    (*block)->next_block = next;
    (*block)->size = size;
    (*block)->start = start;

    return TRUE;
}

bool_ct firstfit_add_block(firstfit_pool_ct *pool, addr_ct start, size_ct size)
{
    if (0 == size)
    {
        return FALSE;
    }

    firstfit_block_ct *prev_block = NULL;
    firstfit_block_ct *curr_block = pool->first_block;
    addr_ct end = (s8_ct *)start + size;
    while (NULL != curr_block && (curr_block->start + curr_block->size) <= start)
    {
        prev_block = curr_block;
        curr_block = curr_block->next_block;
    }

    bool_ct can_return = FALSE;
    if (NULL != curr_block)
    {
        // Check if the new block overlaps with current block:
        if (start == curr_block->start || end == (curr_block->start + curr_block->size) ||
            IS_BETWEEN(start, curr_block->start, (curr_block->start + curr_block->size)) ||
            IS_BETWEEN(end, curr_block->start, (curr_block->start + curr_block->size)) ||
            IS_BETWEEN(curr_block->start, start, end) || IS_BETWEEN((curr_block->start + curr_block->size), start, end))
        {
            return FALSE;
        }

        // Coalesce the right-hand side block:
        if (end == curr_block->start)
        {
            curr_block->start = start;
            curr_block->size += size;
            can_return = TRUE;
        }
    }

    // Coalesce the left-hand side block:
    if (NULL != prev_block)
    {
        addr_ct prev_block_end = (s8_ct *)prev_block->start + prev_block->size;
        if (start == prev_block_end)
        {
            if (NULL != curr_block && start == curr_block->start)
            {
                prev_block->size += curr_block->size;
                prev_block->next_block = curr_block->next_block;
                pool->xfree(curr_block);
            }
            else
            {
                prev_block->size += size;
            }

            can_return = TRUE;
        }
    }

    if (can_return)
    {
        return TRUE;
    }

    // No coalescing is possible:
    firstfit_block_ct *new_block = NULL;
    bool_ct ret = create_block(pool, &new_block, curr_block, size, start);
    if (!ret)
    {
        return FALSE;
    }

    if (NULL == prev_block)
    {
        pool->first_block = new_block;
    }
    else
    {
        prev_block->next_block = new_block;
    }

    return TRUE;
}

static inline void insert_alloc_info(firstfit_pool_ct *pool, firstfit_alloc_info_ct *info, addr_ct start, size_ct size)
{
    info->size = size;
    info->start = start;
    info->next = pool->first_alloc_info;
    pool->first_alloc_info = info;
}

static inline addr_ct alloc_common(firstfit_pool_ct *pool, ffit_alloc_constraint_ct constraint, size_ct size,
                                   addr_ct start_addr, size_ct alignment)
{
    firstfit_block_ct *curr_block = pool->first_block;
    firstfit_block_ct *prev_block = NULL;

    firstfit_alloc_info_ct *info = pool->xmalloc(sizeof(firstfit_alloc_info_ct));
    if (NULL == info)
    {
        return NULL;
    }

    // These are used for potential splits if constraint is ALIGNMENT or START_ADDRESS:
    firstfit_block_ct *block_if_split = NULL;
    addr_ct start_if_split;
    size_ct max_right_offset_if_split = 0;
    addr_ct ret_addr = NULL;
    bool_ct found_block = FALSE;

    while (NULL != curr_block)
    {
        if (curr_block->size >= size &&
            (constraint == NO_CONSTRAINTS || (constraint == START_ADDRESS && curr_block->start == start_addr) ||
             (constraint == ALIGNMENT && IS_ADDR_ALIGNED(curr_block->start, alignment))))
        {
            addr_ct block_start = curr_block->start;
            if (curr_block->size > size)
            {
                curr_block->size -= size;
                curr_block->start = (s8_ct *)curr_block->start + size;
            }
            else if (curr_block->size == size)
            {
                if (NULL == prev_block)
                {
                    pool->first_block = curr_block->next_block;
                }
                else
                {
                    prev_block->next_block = curr_block->next_block;
                }

                pool->xfree(curr_block);
            }

            ret_addr = block_start;
            found_block = TRUE;

            break;
        }

        if (constraint == ALIGNMENT || constraint == START_ADDRESS)
        {
            addr_ct aligned_start = start_addr;
            if (constraint == ALIGNMENT)
            {
                aligned_start = ALIGN_UP_TO(curr_block->start, alignment);
            }

            size_ct right_offset =
                (PTR_TO_SIZE(curr_block->start) + curr_block->size) - (PTR_TO_SIZE(aligned_start) + size);
            if (aligned_start >= curr_block->start &&
                PTR_TO_SIZE(aligned_start) + size <= PTR_TO_SIZE(curr_block->start) + curr_block->size &&
                right_offset > max_right_offset_if_split)
            {
                block_if_split = curr_block;
                start_if_split = aligned_start;
                max_right_offset_if_split = right_offset;
            }
        }

        prev_block = curr_block;
        curr_block = curr_block->next_block;
    }

    if (!found_block)
    {
        if ((constraint == ALIGNMENT || constraint == START_ADDRESS) && block_if_split != NULL)
        {
            bool_ct ret = TRUE;
            if (max_right_offset_if_split > 0)
            {
                firstfit_block_ct *right_block;
                ret = create_block(pool, &right_block, block_if_split->next_block, max_right_offset_if_split,
                                   (s8_ct *)start_if_split + size);
                if (ret)
                {
                    block_if_split->next_block = right_block;
                }
            }

            if (ret)
            {
                block_if_split->size = (PTR_TO_SIZE(start_if_split)) - (PTR_TO_SIZE(block_if_split->start));

                ret_addr = start_if_split;
                found_block = TRUE;
            }
        }
    }

    if (found_block)
    {
        insert_alloc_info(pool, info, ret_addr, size);
    }
    else
    {
        pool->xfree(info);
    }

    return ret_addr;
}

addr_ct firstfit_alloc(firstfit_pool_ct *pool, size_ct size)
{
    if (0 == size)
    {
        return NULL;
    }

    return alloc_common(pool, NO_CONSTRAINTS, size, NULL, 0);
}

addr_ct firstfit_alloc_with_start_addr(firstfit_pool_ct *pool, size_ct size, addr_ct start_addr)
{
    if (0 == size)
    {
        return NULL;
    }

    return alloc_common(pool, START_ADDRESS, size, start_addr, 0);
}

addr_ct firstfit_alloc_with_alignment(firstfit_pool_ct *pool, size_ct size, size_ct alignment)
{
    if (0 == size || 0 == alignment)
    {
        return NULL;
    }

    return alloc_common(pool, ALIGNMENT, size, NULL, alignment);
}

bool_ct firstfit_free(firstfit_pool_ct *pool, addr_ct addr)
{
    if (NULL == addr)
    {
        return FALSE;
    }

    firstfit_alloc_info_ct *curr_info = pool->first_alloc_info;
    firstfit_alloc_info_ct *prev_info = NULL;
    size_ct allocated_size = 0;
    while (NULL != curr_info)
    {
        if (addr == curr_info->start)
        {
            allocated_size = curr_info->size;
            break;
        }

        prev_info = curr_info;
        curr_info = curr_info->next;
    }

    if (NULL == curr_info)
    {
        return FALSE;
    }

    if (NULL == prev_info)
    {
        pool->first_alloc_info = curr_info->next;
    }
    else
    {
        prev_info->next = curr_info->next;
    }

    pool->xfree(curr_info);

    return firstfit_add_block(pool, addr, allocated_size);
}

addr_ct firstfit_get_first_free_address(firstfit_pool_ct *pool)
{
    return pool->first_block->start;
}