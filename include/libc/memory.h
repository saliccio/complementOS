#pragma once

#include "types.h"

// Transfers desired amount of data from the address pointed by src_ptr to dest_ptr.
void mem_copy(const u8_ct *src_ptr, u8_ct *dest_ptr, u32_ct bytes);

// Similar to mem_copy, but for strings. Stops at null terminator.
void mem_copy_str(const s8_ct *src_ptr, s8_ct *dest_ptr);

// Writes the given data to memory, starting from dest_ptr, for given amount of bytes.
void mem_set(u8_ct *dest_ptr, u8_ct data, u32_ct bytes);

void swap(u8_ct *ptr1, u8_ct *ptr2, u32_ct bytes);