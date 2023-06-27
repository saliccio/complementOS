#include "interrupts.h"
#include <cpu/isr.h>
#include <d_screen.h>
#include <cpu/timer.h>

// Programmable Interval Timer Interrupt
void irq0() {
    static int ticks = 0;
    ticks++;
    if (ticks % TIMER_TICKS_PER_SECOND == 0) {
        printf("One second!\n");
        ticks = 0;
    }
}

void interrupt_setup_handlers() {
    timer_init();
    isr_set_handler(IRQ0, irq0);

    __asm__ __volatile__ ("sti");
}