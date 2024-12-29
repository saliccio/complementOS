#pragma once

// PS/2 keyboard driver implementation

#include <types.h>

// Scancodes
#define KEY_ERROR 0
#define KEY_ESCAPE 1
#define KEY_1 2
#define KEY_2 3
#define KEY_3 4
#define KEY_4 5
#define KEY_5 6
#define KEY_6 7
#define KEY_7 8
#define KEY_8 9
#define KEY_9 10
#define KEY_0 11
#define KEY_HYPHEN 12
#define KEY_PLUS 13
#define KEY_BACKSPACE 14
#define KEY_TAB 15
#define KEY_Q 16
#define KEY_W 17
#define KEY_E 18
#define KEY_R 19
#define KEY_T 20
#define KEY_Y 21
#define KEY_U 22
#define KEY_I 23
#define KEY_O 24
#define KEY_P 25
#define KEY_OPENING_SQUARE_BRACKET 26
#define KEY_CLOSING_SQUARE_BRACKET 27
#define KEY_ENTER 28
#define KEY_LCTRL 29
#define KEY_A 30
#define KEY_S 31
#define KEY_D 32
#define KEY_F 33
#define KEY_G 34
#define KEY_H 35
#define KEY_J 36
#define KEY_K 37
#define KEY_L 38
#define KEY_SEMICOLON 39
#define KEY_SINGLE_QUOTE 40
#define KEY_BACKTICK 41
#define KEY_LSHIFT 42
#define KEY_BACKSLASH 43
#define KEY_Z 44
#define KEY_X 45
#define KEY_C 46
#define KEY_V 47
#define KEY_B 48
#define KEY_N 49
#define KEY_M 50
#define KEY_COMMA 51
#define KEY_DOT 52
#define KEY_SLASH 53
#define KEY_RSHIFT 54
#define KEY_KEYPAD_ASTERISK 55
#define KEY_KEYPAD_LALT 56
#define KEY_KEYPAD_SPACE 57
#define KEYBOARD_LAST_SCANCODE KEY_KEYPAD_SPACE

// Key up event scancode = key scancode + 0x80
#define KEY_UP(key) key+0x80

#define KEYBOARD_SCANCODE_PORT 0x60

#define KEY_MAX_STR_LENGTH 22

// Translates given scancode to the corresponding key's string equivalent, and writes to the pointer 'dest'. Does not support KEYUP events.
void keyboard_scancode_to_str(u8 scancode, char *dest);