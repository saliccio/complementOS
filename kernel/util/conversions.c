#include "conversions.h"
#include <util/array.h>

int int_to_str(int src, char *dest) {
    bool is_negative = src < 0;
    if (is_negative) {
        src = -src;  // Absolute value
    }
    
    int i = 0;
    do {
        dest[i++] = '0' + (src % 10);  // ASCII value = ASCII of '0' + digit
    } while ((src /= 10) > 0);
    
    if (is_negative) {
        dest[i++] = '-';
    }

    // Reverse it
    reverse(dest, i);

    dest[i] = '\0';

    return i;
}

int int_to_str_truncate_0(int src, char *dest) {
    bool is_negative = src < 0;
    if (is_negative) {
        src = -src;  // Absolute value
    }
    
    int i = 0;
    do {
        dest[i++] = '0' + (src % 10);  // ASCII value = ASCII of '0' + digit
    } while ((src /= 10) > 0);
    
    if (is_negative) {
        dest[i++] = '-';
    }

    int last_0_index = -1;
    for (int j = 0; j < i - 1; j++) {  // Get the index of truncation for 0 in the reversed string
        if (dest[j] != '0') {
            break;
        }
        last_0_index = j;
    }

    if (last_0_index != -1) {
        dest[last_0_index] = '\0';  // Truncates the zeros at the start
        i -= last_0_index + 1;  // Update the string size
    } else {
        dest[i] = '\0';  // If there is no 0, terminator is just at the end
    }

    // Reverse it
    reverse(dest, i);

    return i;
}

int float_to_str(float src, char *dest) {
    int int_part = (int) src;
    float float_part = src - (float) int_part;
    
    int int_part_end_index = int_to_str(int_part, dest);
    dest[int_part_end_index] = '.';

    int fractional_part = (float_part * FLOAT_FRACTIONAL_SHIFT_FACTOR);
    int fractional_part_end_index = int_to_str_truncate_0(fractional_part, dest + int_part_end_index + 1);

    return int_part_end_index + fractional_part_end_index + 1;
}