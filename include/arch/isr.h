#pragma once

#include <arch/idt.h>
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

#define EXC0 0
#define EXC1 1
#define EXC2 2
#define EXC3 3
#define EXC4 4
#define EXC5 5
#define EXC6 6
#define EXC7 7
#define EXC8 8
#define EXC9 9
#define EXC10 10
#define EXC11 11
#define EXC12 12
#define EXC13 13
#define EXC14 14
#define EXC15 15
#define EXC16 16
#define EXC17 17
#define EXC18 18
#define EXC19 19
#define EXC20 20
#define EXC21 21
#define EXC22 22
#define EXC23 23
#define EXC24 24
#define EXC25 25
#define EXC26 26
#define EXC27 27
#define EXC28 28
#define EXC29 29
#define EXC30 30
#define EXC31 31
#define EXCVEC_PAGEFAULT 14

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

#define PIC_SLAVE_STARTING_IRQ_VECTOR IRQ8

// Low-level exception handler entries (defined in interrupts.asm)
extern void _exc0();
extern void _exc1();
extern void _exc2();
extern void _exc3();
extern void _exc4();
extern void _exc5();
extern void _exc6();
extern void _exc7();
extern void _exc8();
extern void _exc9();
extern void _exc10();
extern void _exc11();
extern void _exc12();
extern void _exc13();
extern void _exc14();
extern void _exc15();
extern void _exc16();
extern void _exc17();
extern void _exc18();
extern void _exc19();
extern void _exc20();
extern void _exc21();
extern void _exc22();
extern void _exc23();
extern void _exc24();
extern void _exc25();
extern void _exc26();
extern void _exc27();
extern void _exc28();
extern void _exc29();
extern void _exc30();
extern void _exc31();
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

typedef struct {
   u32 ds; // Data segment entry offset
   u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by 'pusha' instruction
   u32 int_no, err_code; // Interrupt number and error code (if exists)
   u32 eip, cs, eflags, useresp, ss; // Pushed by the processor automatically
} __attribute__((packed)) isr_registers_t;

typedef void (*isr_handler_t) (isr_registers_t);

// General ISR handler function. Called from interrupts.asm, and the parameter is pulled from the stack.
void isr_general_handler(isr_registers_t registers);

// Set a handler function for given interrupt number.
void isr_set_handler(u8 int_no, isr_handler_t handler);

void interrupts_enable();