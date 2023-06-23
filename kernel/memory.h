#pragma once

// Transfers desired amount of data from the address pointed by src_ptr to dest_ptr.
void mem_copy(char *src_ptr, char *dest_ptr, int bytes);

// Writes the given data to memory, starting from dest_ptr, for given amount of bytes.
void mem_set(char *dest_ptr, char data, int bytes);