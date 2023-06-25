#include "memory.h"

void mem_copy(u8 *src_ptr, u8 *dest_ptr, int bytes) {
    for (int i = 0; i < bytes; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

void mem_set(u8 *dest_ptr, u8 data, int bytes) {
    for (int i = 0; i < bytes; i++) {
        dest_ptr[i] = data;
    }
}