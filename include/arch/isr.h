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

typedef struct isr_registers
{
    u64_ct r15, r14, r13, r12, r11, r10, r9, r8;
    u64_ct rbp, rdi, rsi, rdx, rcx, rbx, rax;
    u64_ct int_no, error_code;
    u64_ct rip, cs, rflags, rsp, ss; // Automatically pushed by CPU for interrupts
} __attribute__((packed)) isr_registers_ct;

typedef void (*isr_handler_ct)(isr_registers_ct *);

// Set a handler function for given interrupt number.
void isr_set_handler(u8_ct int_no, isr_handler_ct handler);