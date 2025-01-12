#include <idt.h>
#include <pit.h>
#include <arch/ports.h>
#include "archAsm.h"

/*
TODO: Move into drivers
void timer_interrupt() {
    static unsigned int ticks = 0;
    ticks++;
    if (ticks % TIMER_TICKS_PER_SECOND == 0) {
        //printf("One second!\n");
    }
}

void keyboard_interrupt() {
    u8_ct key_scancode = port_read_byte(KEYBOARD_SCANCODE_PORT);
    bool_ct is_keyup_event = key_scancode > KEYBOARD_LAST_SCANCODE;

    if (!is_keyup_event) {
        char key_str[KEY_MAX_STR_LENGTH];
        keyboard_scancode_to_str(key_scancode, key_str);
        printf(key_str);
    }
}
*/

void interrupts_init() {
    idt_init();

    ASM("sti");  // Enable interrupts again
}