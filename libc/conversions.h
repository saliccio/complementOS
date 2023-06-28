#pragma once

#include <types.h>

#define FLOAT_FRACTIONAL_SHIFT_FACTOR 10000000  // This determines the maximum digits converted in the fractional part

// Converts given integer src to string and writes it to given pointer. Returns the index of the null terminator.
int int_to_str(int src, char *dest);
// Converts given float src to string and writes it to given pointer. Returns the index of the null terminator.
int float_to_str(float src, char *dest);