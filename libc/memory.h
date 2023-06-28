#pragma once

#include "types.h"

// Transfers desired amount of data from the address pointed by src_ptr to dest_ptr.
void mem_copy(const u8 *src_ptr, u8 *dest_ptr, int bytes);
// Similar to mem_copy, but for strings. Stops at null terminator.
void mem_copy_str(const s8 *src_ptr, s8 *dest_ptr);
// Writes the given data to memory, starting from dest_ptr, for given amount of bytes.
void mem_set(u8 *dest_ptr, u8 data, int bytes);