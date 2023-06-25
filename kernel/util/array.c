#include "array.h"

void reverse(u8 *arr, int size) {
    for(int i = 0; i < size; i++) {
        arr[i] = arr[size - i - 1];
    }
}

int first_index(u8 *arr, int size, u8 token) {
    for(int i = 0; i < size; i++) {
        if (arr[i] == token) {
            return i;
        }
    }
    return -1;
}

int last_index(u8 *arr, int size, u8 token) {
    for(int i = size - 1; i >= 0; i--) {
        if (arr[i] == token) {
            return i;
        }
    }
    return -1;
}