#pragma once

#include "errCodes.h"

// Prints the given string, starts at the cursor position.
void vga_print(const char *string);

// Prints considering given format and ordered parameters.
void vga_printf(const char *format, ...);

// Clears the whole screen by writing blank characters at all positions.
void vga_screen_clear();