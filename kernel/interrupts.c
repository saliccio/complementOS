#include "interrupts.h"
#include <cpu/isr.h>
#include <d_screen.h>
#include <cpu/timer.h>
#include <cpu/io.h>
#include <d_keyboard.h>

// Programmable Interval Timer Interrupt
static void irq0() {
    static int ticks = 0;
    ticks++;
    if (ticks % TIMER_TICKS_PER_SECOND == 0) {
        //printf("One second!\n");
    }
}

// Keyboard Interrupt
static void irq1() {
    u8 key_scancode = port_read_byte(KEYBOARD_SCANCODE_PORT);
    bool is_keyup_event = key_scancode > KEYBOARD_LAST_SCANCODE;

    if (!is_keyup_event) {
        char key_str[KEY_MAX_STR_LENGTH];
        keyboard_scancode_to_str(key_scancode, key_str);
        printf(key_str);
    }
}

void interrupt_setup_handlers() {
    timer_init();
    isr_set_handler(IRQ0, irq0);

    isr_set_handler(IRQ1, irq1);

    __asm__ __volatile__ ("sti");  // Enable interrupts again
}