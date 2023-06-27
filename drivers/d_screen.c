#include "d_screen.h"
#include <io.h>
#include <util/memory.h>
#include <types.h>
#include <util/conversions.h>
#include <stdarg.h>

int screen_setup() {
	screen_clear();
	return 1;
}

int screen_get_cursor_offset() {
    // register 14: high byte of cursor offset
    // register 15: low byte of cursor offset
    port_write_byte(SCREEN_CTRL_PORT, 14);  // first, select register 14
    int cursor_offset = port_read_byte(SCREEN_DATA_PORT) << 8;  // read the high byte and left-shift by 8 bits (since high byte is read)
    port_write_byte(SCREEN_CTRL_PORT, 15);  // then, select register 15
    cursor_offset += port_read_byte(SCREEN_DATA_PORT);  // read the low byte
    cursor_offset *= 2;  // each character in VGA is 2 bytes, so multiply it by two (convert it from char offset to address offset)
    return cursor_offset;
}

void screen_set_cursor_offset(int offset) {
    offset /= 2;  // convert the offset from address offset to char offset

    port_write_byte(SCREEN_CTRL_PORT, 14);
    port_write_byte(SCREEN_DATA_PORT, (u8)(offset >> 8));  // to write the upper byte, right-shift the given offset by 8 bits
    port_write_byte(SCREEN_CTRL_PORT, 15);
    port_write_byte(SCREEN_DATA_PORT, (u8)(offset));
}

void screen_scroll_below() {
    // bring all rows back by one
    for (int i = 1; i < MAX_ROWS; i++) {
        char *src_ptr = (char *)VIDEO_ADDRESS + 2 * i * MAX_COLS;
        char *dest_ptr = (char *)VIDEO_ADDRESS + 2 * (i - 1) * MAX_COLS;
        mem_copy(src_ptr, dest_ptr, 2 * MAX_COLS);
    }

    char *last_row_ptr = (char *)VIDEO_ADDRESS + 2 * (MAX_ROWS - 1) * MAX_COLS;  // starting address of the last row
    mem_set(last_row_ptr, 0, 2 * MAX_COLS);  // clears the last row
}

void print_char_at(int row, int column, char character, char attribute) {
	u8 *video_memory_ptr = (u8 *) VIDEO_ADDRESS;

    if (attribute == DEFAULT_ATTR_BYTE) {
        attribute = COLOR_DEFAULT;
    }

    int offset;
    if (row >= 0 && column >= 0) {
        offset = 2 * (row * MAX_COLS + column);
    } else {
        offset = screen_get_cursor_offset();
        row = offset / (2 * MAX_COLS);
    }

    if (character == '\n') {  // process newline character
        offset = 2 * (row + 1) * MAX_COLS;  // set the offset to the beginning of the next row
    } else if (character == '\t') {  // process carriage return character
        offset = 2 * (row * MAX_COLS);  // set the offset to the beginning of the current row
    } else {  // process regular characters
        video_memory_ptr[offset++] = character;
        video_memory_ptr[offset++] = attribute;
    }

    if (offset >= 2 * MAX_COLS * MAX_ROWS) {  // if the offset is overflowed
        screen_scroll_below();  // scroll the screen to allow an empty line to show up at the bottom
        offset -= 2 * MAX_COLS;  // bring the offset back to the last line
    }

    screen_set_cursor_offset(offset);
}

void print_char(char character, char attribute) {
    print_char_at(-1, -1, character, attribute);
}

void print_at(int row, int column, const char* string) {
    if (row >= 0 && column >= 0) {  // update the cursor position if row and column are valid
        screen_set_cursor_offset(2 * (row * MAX_COLS + column));
    }

    int i = 0;
    while (string[i] != 0) {  // print until a termination character is encountered
        print_char_at(-1, -1, string[i], DEFAULT_ATTR_BYTE);
        i++;
    }
}

void print(const char *string) {
    print_at(-1, -1, string);
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int i = 0;
    char c;
    bool is_in_format_mode = false;
    while ((c = format[i]) != 0) {
        if (is_in_format_mode) {
            if (c == '%') {  // Process escape %
                print_char('%', DEFAULT_ATTR_BYTE);
                is_in_format_mode = false;
            } else if (c == 'c') {  // Process chars
                int arg = va_arg(args, int);
                print_char(arg, DEFAULT_ATTR_BYTE);
                is_in_format_mode = false;
            } else if (c == 'd') {  // Process integers
                int arg = va_arg(args, int);
                char int_str[INT_MAX_CHARS_10];
                int_to_str(arg, int_str);
                print(int_str);
                is_in_format_mode = false;
            } else if (c == 'f') {  // Process floats
                double arg = va_arg(args, double);
                char float_str[FLOAT_MAX_CHARS_10];
                float_to_str(arg, float_str);
                print(float_str);
                is_in_format_mode = false;
            } else if (c == 's') {  // Process strings
                const char *arg = va_arg(args, const char*);
                print(arg);
                is_in_format_mode = false;
            }
        } else if (c == '%') {  // Process format mode entry
            is_in_format_mode = true;
        } else {  // Process regular chars in the format string
            print_char(c, DEFAULT_ATTR_BYTE);
        }
        i++;
    }

    va_end(args);
}

void screen_clear() {
    int row, column;
    for (row = 0; row < MAX_ROWS; row++) {
        for (column = 0; column <  MAX_COLS; column++) {
            print_char_at(row, column, ' ', DEFAULT_ATTR_BYTE);
        }
    }

    screen_set_cursor_offset(0);
}