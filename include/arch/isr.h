#pragma once

#include <types.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47
#define INTVEC_TIMER IRQ0
#define INTVEC_KEYBOARD IRQ1
#define INTVEC_MOUSE IRQ12

typedef struct {
   u32_ct ds; // Data segment entry offset
   u32_ct edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by 'pusha' instruction
   u32_ct int_no, err_code; // Interrupt number and error code (if exists)
   u32_ct eip, cs, eflags, useresp, ss; // Pushed by the processor automatically
} __attribute__((packed)) isr_registers_t;

typedef void (*isr_handler_t) (isr_registers_t);

// Set a handler function for given interrupt number.
void isr_set_handler(u8_ct int_no, isr_handler_t handler);