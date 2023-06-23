#include "memory.h"

void mem_copy(char *src_ptr, char *dest_ptr, int bytes) {
    for (int i = 0; i < bytes; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

void mem_set(char *dest_ptr, char data, int bytes) {
    for (int i = 0; i < bytes; i++) {
        dest_ptr[i] = data;
    }
}