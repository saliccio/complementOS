#pragma once

#include <types.h>

typedef struct
{
    bool_ct *data;
    u32_ct size;
} memory_bitmap_t;

void allocator_init();
void *malloc(u32_ct size);