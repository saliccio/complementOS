#pragma once

#include <types.h>

#define FLOAT_MAX_CHARS_10 57 // Maximum number of chars needed to represent float numbers in base 10
#define INT_MAX_CHARS_10 11   // Maximum number of chars needed to represent integer numbers in base 10
#define INT_MAX_CHARS_16 12   // Maximum number of chars needed to represent integer numbers in base 16

#define FLOAT_FRACTIONAL_SHIFT_FACTOR                                                                                  \
    10000000 // This determines the maximum number of digits converted in the fractional part

// Converts given unsigned integer src to string and writes it to given pointer. Returns the index of the null
// terminator.
int uint_to_str(unsigned int src, char *dest);

// Converts given integer src to string and writes it to given pointer. Returns the index of the null terminator.
int int_to_str(int src, char *dest);

// Converts given float src to string and writes it to given pointer. Returns the index of the null terminator.
int float_to_str(float src, char *dest);

// Converts given unsigned decimal value src to hexadecimal and writes it to given pointer. Returns the index of the
// null terminator.
int udec_to_hex(unsigned int src, char *dest);

// Converts given decimal value src to hexadecimal and writes it to given pointer. Returns the index of the null
// terminator.
int dec_to_hex(int src, char *dest);