#pragma once

#include "types.h"

bool_ct mem_area_init();

addr_ct mem_area_alloc(size_ct size);

addr_ct mem_area_alloc_with_start_addr(size_ct size, addr_ct start_addr);

addr_ct mem_area_alloc_with_alignment(size_ct size, size_ct alignment);