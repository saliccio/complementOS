#include "isr.h"
#include "idt.h"
#include <io.h>
#include <d_screen.h>
#include <util/conversions.h>

void idt_init() {
    idt_assign_isr(0, (u32)_isr0);
    idt_assign_isr(1, (u32)_isr1);
    idt_assign_isr(2, (u32)_isr2);
    idt_assign_isr(3, (u32)_isr3);
    idt_assign_isr(4, (u32)_isr4);
    idt_assign_isr(5, (u32)_isr5);
    idt_assign_isr(6, (u32)_isr6);
    idt_assign_isr(7, (u32)_isr7);
    idt_assign_isr(8, (u32)_isr8);
    idt_assign_isr(9, (u32)_isr9);
    idt_assign_isr(10, (u32)_isr10);
    idt_assign_isr(11, (u32)_isr11);
    idt_assign_isr(12, (u32)_isr12);
    idt_assign_isr(13, (u32)_isr13);
    idt_assign_isr(14, (u32)_isr14);
    idt_assign_isr(15, (u32)_isr15);
    idt_assign_isr(16, (u32)_isr16);
    idt_assign_isr(17, (u32)_isr17);
    idt_assign_isr(18, (u32)_isr18);
    idt_assign_isr(19, (u32)_isr19);
    idt_assign_isr(20, (u32)_isr20);
    idt_assign_isr(21, (u32)_isr21);
    idt_assign_isr(22, (u32)_isr22);
    idt_assign_isr(23, (u32)_isr23);
    idt_assign_isr(24, (u32)_isr24);
    idt_assign_isr(25, (u32)_isr25);
    idt_assign_isr(26, (u32)_isr26);
    idt_assign_isr(27, (u32)_isr27);
    idt_assign_isr(28, (u32)_isr28);
    idt_assign_isr(29, (u32)_isr29);
    idt_assign_isr(30, (u32)_isr30);
    idt_assign_isr(31, (u32)_isr31);

    // Remapping of Intel 8259 Programmable Interrupt Controller in cascade mode (for IRQs 0-7, remap from 0x8-0xF to 0x20-0x27 to avoid conflicts with ISRs) (for IRQs 8-15, remap from 0x70-0x77 to 0x28-0x2F to ensure sequentiality)
    port_write_byte(PIC_MASTER_COMMAND_PORT, PIC_INIT_COMMAND);  // Initialize master PIC
    port_write_byte(PIC_SLAVE_COMMAND_PORT, PIC_INIT_COMMAND);  // Initialize slave PIC
    port_write_byte(PIC_MASTER_DATA_PORT, PIC_MASTER_VECTOR_OFFSET);  // Remap IRQs 0-7
    port_write_byte(PIC_SLAVE_DATA_PORT, PIC_SLAVE_VECTOR_OFFSET);  // Remap IRQs 8-15
    port_write_byte(PIC_MASTER_DATA_PORT, PIC_SLAVE_IRQ_NO);  // Tell master PIC which IRQ input the slave PIC resides at
    port_write_byte(PIC_SLAVE_DATA_PORT, PIC_SLAVE_CASCADE_IDENTITY);  // Tell Slave PIC its cascade identity
    port_write_byte(PIC_MASTER_DATA_PORT, PIC_8086_MODE);  // Make master PIC use 8086 mode
    port_write_byte(PIC_SLAVE_DATA_PORT, PIC_8086_MODE);  // Make slave PIC use 8086 mode
    port_write_byte(PIC_MASTER_DATA_PORT, 0x0);  // Reset interrupt masks for master PIC
    port_write_byte(PIC_SLAVE_DATA_PORT, 0x0);   // Reset interrupt masks for slave PIC

    idt_assign_isr(32, (u32)_irq0);
    idt_assign_isr(33, (u32)_irq1);
    idt_assign_isr(34, (u32)_irq2);
    idt_assign_isr(35, (u32)_irq3);
    idt_assign_isr(36, (u32)_irq4);
    idt_assign_isr(37, (u32)_irq5);
    idt_assign_isr(38, (u32)_irq6);
    idt_assign_isr(39, (u32)_irq7);
    idt_assign_isr(40, (u32)_irq8);
    idt_assign_isr(41, (u32)_irq9);
    idt_assign_isr(42, (u32)_irq10);
    idt_assign_isr(43, (u32)_irq11);
    idt_assign_isr(44, (u32)_irq12);
    idt_assign_isr(45, (u32)_irq13);
    idt_assign_isr(46, (u32)_irq14);
    idt_assign_isr(47, (u32)_irq15);

    idt_load();
}

void isr_general_handler(isr_registers_t registers) {
    printf("[Interrupt trigger]: %d\n", registers.int_no);

    isr_handler_t isr_specific_handler = isr_handlers[registers.int_no];
    bool is_specific_handler_set = isr_specific_handler != 0;
    if (is_specific_handler_set) {
        isr_specific_handler(registers);
    }
}

isr_handler_t isr_handlers[IDT_NO_OF_ENTRIES];
void isr_set_handler(u8 int_number, isr_handler_t handler) {
    isr_handlers[int_number] = handler;
}

void irq_general_handler(isr_registers_t registers) {
    port_write_byte(PIC_MASTER_COMMAND_PORT, PIC_EOI_COMMAND);  // ALWAYS send an EOI to the master PIC
    if (registers.int_no >= PIC_SLAVE_STARTING_IRQ_VECTOR) {  // If the interrupt is processed by the slave PIC, send an EOI to it
        port_write_byte(PIC_SLAVE_COMMAND_PORT, PIC_EOI_COMMAND);
    }

    isr_handler_t isr_specific_handler = isr_handlers[registers.int_no];
    bool is_specific_handler_set = isr_specific_handler != 0;
    if (is_specific_handler_set) {
        isr_specific_handler(registers);
    }
}