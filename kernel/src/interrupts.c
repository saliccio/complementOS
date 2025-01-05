#include "interrupts.h"
#include <isr.h>
#include <timer.h>
#include <io.h>
#include <d_screen.h>
#include <d_keyboard.h>
#include <d_mouse.h>

void timer_interrupt() {
    static unsigned int ticks = 0;
    ticks++;
    if (ticks % TIMER_TICKS_PER_SECOND == 0) {
        //printf("One second!\n");
    }
}

void keyboard_interrupt() {
    u8 key_scancode = port_read_byte(KEYBOARD_SCANCODE_PORT);
    bool is_keyup_event = key_scancode > KEYBOARD_LAST_SCANCODE;

    if (!is_keyup_event) {
        char key_str[KEY_MAX_STR_LENGTH];
        keyboard_scancode_to_str(key_scancode, key_str);
        printf(key_str);
    }
}

void page_fault_interrupt() {
    printf("Page Fault");
}

void interrupt_setup_handlers() {
    timer_init();
    isr_set_handler(IRQ0, timer_interrupt);

    isr_set_handler(IRQ1, keyboard_interrupt);

    isr_set_handler(IRQ12, mouse_interrupt);

    isr_set_handler(IRQ14, page_fault_interrupt);

    __asm__ __volatile__ ("sti");  // Enable interrupts again
}