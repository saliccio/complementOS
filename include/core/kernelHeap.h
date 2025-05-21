#pragma once

#include "types.h"

bool_ct kheap_init(addr_ct start);

addr_ct kmalloc(size_ct size);

addr_ct kmalloc_without_header(size_ct size);

void kfree(addr_ct addr);

void kfree_without_header(addr_ct addr, size_ct size);

size_ct kheap_get_free_space();