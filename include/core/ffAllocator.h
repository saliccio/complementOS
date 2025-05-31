#pragma once

#include "types.h"

typedef struct firstfit_block
{
    size_ct size;
    addr_ct start;
    struct firstfit_block *next_block;
} firstfit_block_ct;

typedef struct firstfit_pool
{
    firstfit_block_ct *first_block;
    addr_ct (*xmalloc)(size_ct size);
    void (*xfree)(addr_ct addr);
} firstfit_pool_ct;

void firstfit_init(firstfit_pool_ct *pool, addr_ct (*xmalloc)(size_ct size), void (*xfree)(addr_ct addr));

bool_ct firstfit_add_block(firstfit_pool_ct *pool, addr_ct start, size_ct size);

addr_ct firstfit_alloc(firstfit_pool_ct *pool, size_ct size);

addr_ct firstfit_alloc_with_alignment(firstfit_pool_ct *pool, size_ct size, size_ct alignment);

addr_ct firstfit_alloc_with_start_addr(firstfit_pool_ct *pool, size_ct size, addr_ct start_addr);

void firstfit_force_alloc(firstfit_pool_ct *pool, size_ct size, addr_ct start_addr);

bool_ct firstfit_free(firstfit_pool_ct *pool, addr_ct addr, size_ct size);

addr_ct firstfit_get_first_free_address(firstfit_pool_ct *pool);