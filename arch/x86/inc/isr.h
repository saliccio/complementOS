#pragma once

#include "arch/isr.h"

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
#define EXC32 48
#define EXCVEC_PAGEFAULT 14

// Low-level exception handler entries
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
extern void _exc32();
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

// General ISR handler function. Called from interrupts.asm, and the parameter is pulled from the stack.
void isr_general_handler(isr_registers_ct registers);

void isr_set_handler(u8_ct int_no, isr_handler_ct handler);