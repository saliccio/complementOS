#include "isr.h"
#include "idt.h"
#include <d_screen.h>
#include <util/conversions.h>

void idt_init() {
    idt_assign_isr(0, (u32)isr0);
    idt_assign_isr(1, (u32)isr1);
    idt_assign_isr(2, (u32)isr2);
    idt_assign_isr(3, (u32)isr3);
    idt_assign_isr(4, (u32)isr4);
    idt_assign_isr(5, (u32)isr5);
    idt_assign_isr(6, (u32)isr6);
    idt_assign_isr(7, (u32)isr7);
    idt_assign_isr(8, (u32)isr8);
    idt_assign_isr(9, (u32)isr9);
    idt_assign_isr(10, (u32)isr10);
    idt_assign_isr(11, (u32)isr11);
    idt_assign_isr(12, (u32)isr12);
    idt_assign_isr(13, (u32)isr13);
    idt_assign_isr(14, (u32)isr14);
    idt_assign_isr(15, (u32)isr15);
    idt_assign_isr(16, (u32)isr16);
    idt_assign_isr(17, (u32)isr17);
    idt_assign_isr(18, (u32)isr18);
    idt_assign_isr(19, (u32)isr19);
    idt_assign_isr(20, (u32)isr20);
    idt_assign_isr(21, (u32)isr21);
    idt_assign_isr(22, (u32)isr22);
    idt_assign_isr(23, (u32)isr23);
    idt_assign_isr(24, (u32)isr24);
    idt_assign_isr(25, (u32)isr25);
    idt_assign_isr(26, (u32)isr26);
    idt_assign_isr(27, (u32)isr27);
    idt_assign_isr(28, (u32)isr28);
    idt_assign_isr(29, (u32)isr29);
    idt_assign_isr(30, (u32)isr30);
    idt_assign_isr(31, (u32)isr31);

    idt_load();
}

void isr_general_handler(isr_registers_t registers) {
    print("[Interrupt trigger]: ");
    char interrupt_no_str[3];
    int_to_str(registers.int_no, interrupt_no_str);
    print(interrupt_no_str);
    print("\n");
}