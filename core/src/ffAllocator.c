#include "ffAllocator.h"
#include "drivers/d_screen.h"
#include "kernelHeap.h"
/**
 * Used for allocations the first-fit allocator itself makes.
 */
#define ALLOC(size) (kmalloc(size))

/**
 * Used for deallocations the first-fit allocator itself makes.
 */
#define DEALLOC(addr) (kfree(addr))

#define IS_ADDR_ALIGNED(addr, alignment) (0 == ((*(size_ct *)&(addr)) & ((alignment)-1)))

#define ALIGN_UP_TO(addr, alignment) ((addr_ct)(((*(size_ct *)&(addr)) + (alignment)-1) & ~((alignment)-1)))

#define IS_BETWEEN(value, min, max) ((value) > (min) && (value) < (max))

typedef enum ffit_alloc_constraint
{
    NO_CONSTRAINTS = 0, // Except size :)
    START_ADDRESS = 1,
    ALIGNMENT = 2,
} ffit_alloc_constraint_ct;

void firstfit_init(firstfit_pool_ct *pool)
{
    pool->first_block = NULL;
}

static inline bool_ct create_block(firstfit_block_ct **block, firstfit_block_ct *next, size_ct size, addr_ct start)
{
    *block = ALLOC(sizeof(firstfit_block_ct));
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
    firstfit_block_ct *curr_block = pool->first_block;
    firstfit_block_ct *new_block = NULL;
    bool_ct ret;
    if (NULL == curr_block)
    {
        ret = create_block(&new_block, NULL, size, start);
        if (!ret)
        {
            return FALSE;
        }

        pool->first_block = new_block;
    }
    else
    {
        addr_ct end = start + size;
        addr_ct curr_block_end;
        firstfit_block_ct *prev_block = NULL;
        while (NULL != curr_block)
        {
            curr_block_end = curr_block->start + curr_block->size;

            // Check if the new block overlaps with the block:
            if (start == curr_block->start || end == curr_block_end ||
                IS_BETWEEN(start, curr_block->start, curr_block_end) ||
                IS_BETWEEN(end, curr_block->start, curr_block_end) || IS_BETWEEN(curr_block->start, start, end) ||
                IS_BETWEEN(curr_block_end, start, end))
            {
                return FALSE;
            }

            if (end < curr_block->start)
            {
                ret = create_block(&new_block, curr_block, size, start);
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

                break;
            }
            else if (end == curr_block->start)
            {
                curr_block->start = start;
                curr_block->size += size;

                break;
            }

            prev_block = curr_block;
            curr_block = curr_block->next_block;
        }

        if (NULL == curr_block)
        {
            ret = create_block(&new_block, NULL, size, start);
            if (!ret)
            {
                return FALSE;
            }

            prev_block->next_block = new_block;
        }

        // Coalescing loop:
        curr_block = pool->first_block->next_block;
        prev_block = pool->first_block;
        firstfit_block_ct *prev_prev_block = NULL;
        while (NULL != curr_block)
        {
            curr_block_end = curr_block->start + curr_block->size;

            if (prev_block->start + prev_block->size == curr_block->start)
            {
                curr_block->size += prev_block->size;
                curr_block->start = prev_block->start;

                if (pool->first_block == prev_block)
                {
                    pool->first_block = curr_block;
                }
                else
                {
                    prev_prev_block->next_block = curr_block;
                }

                DEALLOC(prev_block);
            }

            prev_prev_block = prev_block;
            prev_block = curr_block;
            curr_block = curr_block->next_block;
        }
    }

    return TRUE;
}

static inline addr_ct alloc_common(firstfit_pool_ct *pool, ffit_alloc_constraint_ct constraint, size_ct size,
                                   addr_ct start_addr, size_ct alignment)
{
    size_ct actual_size = size + sizeof(firstfit_block_header_ct);
    firstfit_block_ct *curr_block = pool->first_block;
    firstfit_block_ct *prev_block = NULL;

    // These are used for potential splits if constraint is ALIGNMENT or START_ADDRESS:
    firstfit_block_ct *block_if_split = NULL;
    addr_ct start_if_split;
    size_ct min_right_offset_if_split = ~0;

    while (NULL != curr_block)
    {
        addr_ct ret_addr = curr_block->start + sizeof(firstfit_block_header_ct);
        if (curr_block->size >= actual_size &&
            (constraint == NO_CONSTRAINTS ||
             (constraint == START_ADDRESS && curr_block->start == (start_addr - sizeof(firstfit_block_header_ct))) ||
             (constraint == ALIGNMENT && IS_ADDR_ALIGNED(ret_addr, alignment))))
        {
            size_ct allocated_size;
            firstfit_block_header_ct *header = curr_block->start;

            if (curr_block->size >= actual_size + FFIT_MIN_BLOCK_SIZE)
            {
                allocated_size = actual_size;
                curr_block->size -= actual_size;
                curr_block->start += actual_size;
            }
            else
            {
                allocated_size = curr_block->size;

                if (NULL == prev_block)
                {
                    pool->first_block = curr_block->next_block;
                }
                else
                {
                    prev_block->next_block = curr_block->next_block;
                }

                DEALLOC(curr_block);
            }

            *header = allocated_size;

            return ((addr_ct)header + sizeof(firstfit_block_header_ct));
        }

        if (constraint == ALIGNMENT || constraint == START_ADDRESS)
        {
            addr_ct aligned_start = start_addr - sizeof(firstfit_block_header_ct);
            if (constraint == ALIGNMENT)
            {
                addr_ct curr_block_with_header = curr_block->start + sizeof(firstfit_block_header_ct);
                aligned_start = ALIGN_UP_TO(curr_block_with_header, alignment) - sizeof(firstfit_block_header_ct);
            }

            size_ct right_offset =
                (*(size_ct *)&curr_block->start + curr_block->size) - (*(size_ct *)&aligned_start + actual_size);
            if (aligned_start >= curr_block->start &&
                aligned_start + actual_size <= curr_block->start + curr_block->size &&
                right_offset < min_right_offset_if_split)
            {
                block_if_split = curr_block;
                start_if_split = aligned_start;
                min_right_offset_if_split = right_offset;
            }
        }

        prev_block = curr_block;
        curr_block = curr_block->next_block;
    }

    if ((constraint == ALIGNMENT || constraint == START_ADDRESS) && block_if_split != NULL)
    {
        size_ct allocated_size;

        // Allocate the full right-hand part if the right residual is lower than the minimum block size:
        if (min_right_offset_if_split < FFIT_MIN_BLOCK_SIZE)
        {
            allocated_size = (block_if_split->start + block_if_split->size) - (*(size_ct *)&start_if_split);
        }
        // Need to split the block otherwise:
        else
        {
            allocated_size = actual_size;
            firstfit_block_ct *right_block;
            bool_ct ret = create_block(&right_block, block_if_split->next_block, min_right_offset_if_split,
                                       start_if_split + actual_size);
            if (!ret)
            {
                return NULL;
            }

            block_if_split->next_block = right_block;
        }

        block_if_split->size = (*(size_ct *)&start_if_split) - (*(size_ct *)&block_if_split->start);

        *(firstfit_block_header_ct *)start_if_split = allocated_size;

        return start_if_split + sizeof(firstfit_block_header_ct);
    }

    return NULL;
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

    addr_ct actual_addr = (addr - sizeof(firstfit_block_header_ct));

    firstfit_block_header_ct *block = actual_addr;
    return firstfit_add_block(pool, actual_addr, *block);
}
