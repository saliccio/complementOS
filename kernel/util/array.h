#pragma once

#include <types.h>

// Reverses the given array.
void reverse(u8 *arr, int size);
// Returns the index of first encountered token in an array. Returns -1 if it does not exist.
int first_index(u8 *arr, int size, u8 token);
// Returns the index of last encountered token in an array. Returns -1 if it does not exist.
int last_index(u8 *arr, int size, u8 token);