#pragma once

#include "types.h"

/**
 * Max order (0-indexed) (Largest block size = 2^(BUDDY_MAX_ORDER + BUDDY_FIRST_ORDER_OFFSET))
 */
#define BUDDY_MAX_ORDER 15

#define BUDDY_FIRST_ORDER_OFFSET 5

/**
 * Smallest allocatable block
 */
#define BUDDY_MIN_BLOCK_SIZE (1 << BUDDY_FIRST_ORDER_OFFSET)

/**
 * Largest allocatable block
 */
#define BUDDY_MAX_BLOCK_SIZE ((1 << (BUDDY_MAX_ORDER + BUDDY_FIRST_ORDER_OFFSET)))

typedef size_ct buddy_block_header_ct;

typedef struct buddy_block
{
    buddy_block_header_ct size;
    struct buddy_block *prev_free_block;
    struct buddy_block *next_free_block;
} buddy_block_ct;

typedef struct buddy_pool
{
    addr_ct pool_start;
    size_ct total_used;
    buddy_block_ct *free_lists[BUDDY_MAX_ORDER + 1];
} buddy_pool_ct;

bool_ct buddy_init(buddy_pool_ct *pool, addr_ct start);

addr_ct buddy_alloc(buddy_pool_ct *pool, size_ct size);

void buddy_free(buddy_pool_ct *pool, addr_ct addr);

size_ct buddy_get_free_space(buddy_pool_ct *pool);