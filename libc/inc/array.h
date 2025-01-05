#pragma once

#include <types.h>

// Reverses the given array.
void reverse(s8 *arr, int size);

// Reverses the given string (until '\0').
void reverse_str(s8 *arr);

// Returns the index of first encountered token in an array. Returns -1 if it does not exist.
int first_index(s8 *arr, int size, s8 token);

// Returns the index of last encountered token in an array. Returns -1 if it does not exist.
int last_index(s8 *arr, int size, s8 token);

// Returns the length of given string.
int str_len(const char *str);

void sort(void* arr, u32 element_size, u32 no_of_elements, bool compare (const void*, const void*));
void sort_ints(s32* arr, u32 no_of_elements, bool ascending);
void sort_floats(float* arr, u32 no_of_elements, bool ascending);