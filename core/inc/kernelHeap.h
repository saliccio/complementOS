#pragma once

#include "types.h"

extern addr_ct _kheap_start;
extern addr_ct _kheap_end;

bool_ct kheap_init();

addr_ct kmalloc(size_ct size);

void kfree(addr_ct addr);

size_ct kheap_get_free_space();