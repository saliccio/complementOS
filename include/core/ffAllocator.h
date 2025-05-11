#pragma once

#include "types.h"

typedef struct firstfit_block
{
    size_ct size;
    addr_ct start;
    struct firstfit_block *next_block;
} firstfit_block_ct;

typedef struct firstfit_alloc_info
{
    size_ct size;
    addr_ct start;
    struct firstfit_alloc_info *next;
} firstfit_alloc_info_ct;

typedef struct firstfit_pool
{
    firstfit_block_ct *first_block;
    firstfit_alloc_info_ct *first_alloc_info;
    addr_ct (*xmalloc)(size_ct size);
    void (*xfree)(addr_ct addr);
} firstfit_pool_ct;

void firstfit_init(firstfit_pool_ct *pool, addr_ct (*xmalloc)(size_ct size), void (*xfree)(addr_ct addr));

bool_ct firstfit_add_block(firstfit_pool_ct *pool, addr_ct start, size_ct size);

addr_ct firstfit_alloc(firstfit_pool_ct *pool, size_ct size);

addr_ct firstfit_alloc_with_start_addr(firstfit_pool_ct *pool, size_ct size, addr_ct start_addr);

addr_ct firstfit_alloc_with_alignment(firstfit_pool_ct *pool, size_ct size, size_ct alignment);

bool_ct firstfit_free(firstfit_pool_ct *pool, addr_ct addr);