#include "memory.h"

void mem_copy(const u8 *src_ptr, u8 *dest_ptr, int bytes) {
    for (int i = 0; i < bytes; i++) {
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
}

void mem_set(u8 *dest_ptr, u8 data, int bytes) {
    for (int i = 0; i < bytes; i++) {
        dest_ptr[i] = data;
    }
}