#include "libc/conversions.h"
#include <libc/array.h>
#include <libc/memory.h>

int uint_to_str(unsigned int src, char *dest) {
    short i = 0;
    do {
        dest[i++] = '0' + (src % 10);  // ASCII value = ASCII of '0' + digit
    } while ((src /= 10) > 0);

    // Reverse it
    reverse(dest, i);

    dest[i] = '\0';

    return i;
}

int int_to_str(int src, char *dest) {
    bool_ct is_negative = src < 0;
    if (is_negative) {
        src *= -1;  // Absolute value
    }
    
    short i = 0;
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
    bool_ct is_negative = src < 0;
    if (is_negative) {
        src = -src;  // Absolute value
    }
    
    dest[0] = '\0';
    int i = 1;
    do {
        dest[i++] = '0' + (src % 10);  // ASCII value = ASCII of '0' + digit
    } while ((src /= 10) > 0);
    
    if (is_negative) {
        dest[i++] = '-';
    }

    int last_0_index = -1;
    for (int j = 1; j < i - 1; j++) {  // Get the index of truncation for 0 in the reversed string
        if (dest[j] != '0') {
            break;
        }
        last_0_index = j;
    }

    if (last_0_index != -1) {
        dest[last_0_index] = '\0';  // Truncates the zeros at the start by replacing the char with null terminator
    }

    // Reverse it
    reverse(dest, i);

    i -= last_0_index + 1;  // Update null terminator index
    return i;
}

int float_to_str(float src, char *dest) {
    int int_part = (int) src;
    float float_part = src - (float) int_part;
    
    int int_part_end_index = int_to_str(int_part, dest);
    dest[int_part_end_index++] = '.';

    int shift_factor = 10;
    while (shift_factor <= FLOAT_FRACTIONAL_SHIFT_FACTOR && (int)(float_part * shift_factor) == 0) {  // Convert leading zeros of the float part
        dest[int_part_end_index++] = '0';
        shift_factor *= 10;
    }
 
    int fractional_part = (int)(float_part * FLOAT_FRACTIONAL_SHIFT_FACTOR);
    if (fractional_part < 0) {  // Ensure absolute value in fractional part
        fractional_part = -fractional_part;
    }
    int fractional_part_end_index = int_to_str_truncate_0(fractional_part, dest + int_part_end_index);

    return int_part_end_index + fractional_part_end_index;
}

int udec_to_hex(unsigned int src, char *dest) {
    if (src == 0) {
        mem_copy_str("0x0", dest);
        return 3;
    }

    short str_index = 0;
    while (src > 0) {
        short remainder = src % 16;
        if (remainder < 10) {
            dest[str_index] = 48 + remainder;
        } else {
            dest[str_index] = 55 + remainder;
        }
        src /= 16;
        str_index++;
    }

    dest[str_index++] = 'x';
    dest[str_index++] = '0';

    reverse(dest, str_index);

    dest[str_index] = '\0';

    return str_index;
}

int dec_to_hex(int src, char *dest) {
    if (src == 0) {
        mem_copy_str("0x0", dest);
        return 3;
    }

    bool_ct is_negative = src < 0;
    if (is_negative) {
        src *= -1;
    }

    short str_index = 0;
    while (src > 0) {
        short remainder = src % 16;
        if (remainder < 10) {
            dest[str_index] = 48 + remainder;
        } else {
            dest[str_index] = 55 + remainder;
        }
        src /= 16;
        str_index++;
    }

    dest[str_index++] = 'x';
    dest[str_index++] = '0';

    if (is_negative) {
        dest[str_index++] = '-';
    }

    reverse(dest, str_index);

    dest[str_index] = '\0';

    return str_index;
}