#include "interrupts.h"
#include <cpu/isr.h>
#include <d_screen.h>
#include <cpu/timer.h>
#include <cpu/io.h>
#include <d_keyboard.h>

void interrupt_setup_handlers() {
    timer_init();
    isr_set_handler(IRQ0, timer_interrupt);

    isr_set_handler(IRQ1, keyboard_interrupt);

    __asm__ __volatile__ ("sti");  // Enable interrupts again
}