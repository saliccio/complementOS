#pragma once

#include "types.h"

void kmemset(u8_ct *dest_ptr, u8_ct data, u32_ct bytes);

u32_ct kmemcmp(const addr_ct s1, const addr_ct s2, ulong_ct n);

void kmemcpy(u8_ct *dst, const u8_ct *src, u64_ct size);