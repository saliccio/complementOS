#pragma once

#include "types.h"

bool_ct kheap_init(addr_ct start);

addr_ct kmalloc(size_ct size);

void kfree(addr_ct addr);

size_ct kheap_get_free_space();