#pragma once

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define COLOR_DEFAULT 0x0F

#define DEFAULT_ATTR_BYTE 0x0  // default color is white

#define SCREEN_CTRL_PORT 0x3D4  // address for the port-mapped control register of the screen device
#define SCREEN_DATA_PORT 0x3D5  // address for the port-mapped data register of the screen device

// Sets the driver up. Returns 1 on success, otherwise 0.
int screen_setup();

// Returns the address offset in VGA memory to write the next character at.
int screen_get_cursor_offset();

// Sets the address offset in VGA memory to write the next character at.
void screen_set_cursor_offset(int offset);

// Scrolls the video buffer below to allow an empty line to show up at the bottom (by moving all rows back by one).
void screen_scroll_below();

// Prints the given character to given position (row and column), with an attribute byte.
// Writes at the cursor if either row or column is negative.
void print_char_at(int row, int column, char character, char attribute);

// Prints the given character at the position of the cursor.
void print_char(char character, char attribute);

// Prints the given string, starts at the given row and column.
void print_at(int row, int column, const char* string);

// Prints the given string, starts at the cursor position.
void print(const char* string);

// Prints considering given format and ordered parameters.
void printf(const char *format, ...);

// Clears the whole screen by writing blank characters at all positions.
void screen_clear();