#include <d_keyboard.h>
#include <libc/memory.h>
#include <drivers/d_screen.h>

void keyboard_scancode_to_str(u8 scancode, char *dest) {
    const char *translation_table[] = {"ERROR", "ESCAPE", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "HYPHEN", "PLUS", "BACKSPACE", "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "OPENING_SQUARE_BRACKET", "CLOSING_SQUARE_BRACKET", "ENTER", "LCTRL", "A", "S", "D", "F", "G", "H", "J", "K", "L", "SEMICOLON", "SINGLE_QUOTE", "BACKTICK", "LSHIFT", "BACKSLASH", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "DOT", "SLASH", "RSHIFT", "KEYPAD_ASTERISK", "KEYPAD_LALT", "KEYPAD_SPACE"};
    if (scancode > KEYBOARD_LAST_SCANCODE) {  // If the scancode is invalid
        *dest = '\0';
        return;
    }

    mem_copy_str(translation_table[scancode], dest);
}