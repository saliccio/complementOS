#include "array.h"
#include "memory.h"
#include "types.h"

void reverse(s8 *arr, int size) {
    for (int i = 0; i < size / 2; i++) {
        s8 tmp = arr[i];
        arr[i] = arr[size - i - 1];
        arr[size - i - 1] = tmp;
    }
}

void reverse_str(s8 *arr) {
    int size = str_len(arr);
    for (int i = 0; i < size / 2; i++) {
        s8 tmp = arr[i];
        arr[i] = arr[size - i - 1];
        arr[size - i - 1] = tmp;
    }
}

int first_index(s8 *arr, int size, s8 token) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == token) {
            return i;
        }
    }
    return -1;
}

int last_index(s8 *arr, int size, s8 token) {
    for (int i = size - 1; i >= 0; i--) {
        if (arr[i] == token) {
            return i;
        }
    }
    return -1;
}

int str_len(const char *str) {
    char c;
    int i = 0;
    while ((c = str[i++]) != '\0');
    
    return i - 1;
}

void sort(void* arr, u32 element_size, u32 no_of_elements, bool compare (const void*, const void*)) {
    u8* arr_u8 = (u8*)arr;
    for (u32 i = 0; i < no_of_elements - 1; i++) {
        for (u32 j = i + 1; j < no_of_elements; j++) {
            int k = j;
            while (k > 0 && compare(&arr_u8[k * element_size], &arr_u8[(k - 1) * element_size])) {
                swap(&arr_u8[k * element_size], &arr_u8[(k - 1) * element_size], element_size);
                k--;
            }
        }
    }
}

bool compare_ints_ascending(const void* ptr1, const void* ptr2) {
    return ((const int*)ptr1 < (const int*)ptr2);
}

bool compare_ints_descending(const void* ptr1, const void* ptr2) {
    return ((const int*)ptr1 > (const int*)ptr2);
}

void sort_ints(s32* arr, u32 no_of_elements, bool ascending) {
    bool (*compare) (const void*, const void*) = ascending ? compare_ints_ascending : compare_ints_descending;
    sort(arr, no_of_elements * sizeof(s32), no_of_elements, compare);
}

bool compare_floats_ascending(const void* ptr1, const void* ptr2) {
    return ((const float*)ptr1 < (const float*)ptr2);
}

bool compare_floats_descending(const void* ptr1, const void* ptr2) {
    return ((const float*)ptr1 > (const float*)ptr2);
}

void sort_floats(float* arr, u32 no_of_elements, bool ascending) {
    bool (*compare) (const void*, const void*) = ascending ? compare_floats_ascending : compare_floats_descending;
    sort(arr, no_of_elements * sizeof(float), no_of_elements, compare);
}