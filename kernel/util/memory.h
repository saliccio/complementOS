#pragma once

#include "types.h"

// Transfers desired amount of data from the address pointed by src_ptr to dest_ptr.
void mem_copy(u8 *src_ptr, u8 *dest_ptr, int bytes);

// Writes the given data to memory, starting from dest_ptr, for given amount of bytes.
void mem_set(u8 *dest_ptr, u8 data, int bytes);