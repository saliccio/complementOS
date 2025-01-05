#include "memory.h"
#include <allocator.h>

void mem_copy(const u8 *src_ptr, u8 *dest_ptr, u32 bytes) {
    for (u32 i = 0; i < bytes; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

void mem_copy_str(const s8 *src_ptr, s8 *dest_ptr) {
    int i = 0;
    char c;
    while ((c = src_ptr[i]) != '\0') {
        dest_ptr[i] = src_ptr[i];
        i++;
    }
    dest_ptr[i] = '\0';
}

void mem_set(u8 *dest_ptr, u8 data, u32 bytes) {
    for (u32 i = 0; i < bytes; i++) {
        dest_ptr[i] = data;
    }
}

void swap(u8 *ptr1, u8 *ptr2, u32 bytes) {
    u8 *temp_array1 = NULL;
    mem_copy(ptr1, temp_array1, bytes);
    mem_copy(ptr2, ptr1, bytes);
    mem_copy(temp_array1, ptr2, bytes);
}