#pragma once

#include <types.h>

// Reverses the given array.
void reverse(s8_ct *arr, int size);

// Reverses the given string (until '\0').
void reverse_str(s8_ct *arr);

// Returns the index of first encountered token in an array. Returns -1 if it does not exist.
int first_index(s8_ct *arr, int size, s8_ct token);

// Returns the index of last encountered token in an array. Returns -1 if it does not exist.
int last_index(s8_ct *arr, int size, s8_ct token);

// Returns the length of given string.
int str_len(const char *str);

void sort(void* arr, u32_ct element_size, u32_ct no_of_elements, bool_ct compare (const void*, const void*));
void sort_ints(s32_ct* arr, u32_ct no_of_elements, bool_ct ascending);
void sort_floats(float* arr, u32_ct no_of_elements, bool_ct ascending);