#pragma once

#include <types.h>

typedef struct {
    bool* data;
    u32 size;
} memory_bitmap_t;

void allocator_init();
void* malloc(u32 size);