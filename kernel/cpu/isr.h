#pragma once

#include <cpu/idt.h>
#include <types.h>

#define PIC_MASTER_COMMAND_PORT 0x20
#define PIC_MASTER_DATA_PORT 0x21
#define PIC_SLAVE_COMMAND_PORT 0xA0
#define PIC_SLAVE_DATA_PORT 0xA1

#define PIC_INIT_COMMAND 0x11
#define PIC_EOI_COMMAND 0x20  // End of interrupt command - MUST BE SENT AFTER PROCESSING THE IRQs!

#define PIC_MASTER_VECTOR_OFFSET 0x20
#define PIC_SLAVE_VECTOR_OFFSET 0x28

#define PIC_SLAVE_IRQ_NO 0x04  // (0000 0100)
#define PIC_SLAVE_CASCADE_IDENTITY 0x02  // (0000 0010)

#define PIC_8086_MODE 0x01

// Low-level interrupt handler entries (defined in interrupts.asm)
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

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

#define PIC_SLAVE_STARTING_IRQ_VECTOR IRQ8

typedef struct {
   u32 ds; // Data segment entry offset
   u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by 'pusha' instruction
   u32 int_no, err_code; // Interrupt number and error code (if exists)
   u32 eip, cs, eflags, useresp, ss; // Pushed by the processor automatically
} __attribute__((packed)) isr_registers_t;

// Initializes IDT by setting up entries.
void idt_init();
// General ISR handler function. Called from interrupts.asm, and the parameter is pulled from the stack.
void isr_general_handler(isr_registers_t registers);

typedef void (*isr_handler_t) (isr_registers_t);
extern isr_handler_t isr_handlers[IDT_NO_OF_ENTRIES];
// Set a handler function for given interrupt number.
void isr_set_handler(u8 int_number, isr_handler_t handler);

// General IRQ handler function. The parameter is pulled from the stack.
void irq_general_handler(isr_registers_t registers);