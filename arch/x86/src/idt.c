#include "idt.h"
#include "isr.h"
#include "gdt.h"
#include <types.h>

static idt_entry_t idt[IDT_NO_OF_ENTRIES];
static idt_ptr_t idt_ptr;

void idt_init() {
    idt_assign_isr(0, (u32)_exc0);
    idt_assign_isr(1, (u32)_exc1);
    idt_assign_isr(2, (u32)_exc2);
    idt_assign_isr(3, (u32)_exc3);
    idt_assign_isr(4, (u32)_exc4);
    idt_assign_isr(5, (u32)_exc5);
    idt_assign_isr(6, (u32)_exc6);
    idt_assign_isr(7, (u32)_exc7);
    idt_assign_isr(8, (u32)_exc8);
    idt_assign_isr(9, (u32)_exc9);
    idt_assign_isr(10, (u32)_exc10);
    idt_assign_isr(11, (u32)_exc11);
    idt_assign_isr(12, (u32)_exc12);
    idt_assign_isr(13, (u32)_exc13);
    idt_assign_isr(14, (u32)_exc14);
    idt_assign_isr(15, (u32)_exc15);
    idt_assign_isr(16, (u32)_exc16);
    idt_assign_isr(17, (u32)_exc17);
    idt_assign_isr(18, (u32)_exc18);
    idt_assign_isr(19, (u32)_exc19);
    idt_assign_isr(20, (u32)_exc20);
    idt_assign_isr(21, (u32)_exc21);
    idt_assign_isr(22, (u32)_exc22);
    idt_assign_isr(23, (u32)_exc23);
    idt_assign_isr(24, (u32)_exc24);
    idt_assign_isr(25, (u32)_exc25);
    idt_assign_isr(26, (u32)_exc26);
    idt_assign_isr(27, (u32)_exc27);
    idt_assign_isr(28, (u32)_exc28);
    idt_assign_isr(29, (u32)_exc29);
    idt_assign_isr(30, (u32)_exc30);
    idt_assign_isr(31, (u32)_exc31);

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

void idt_assign_isr(int interrupt_no, u32 address) {
	idt[interrupt_no].low_offset = low_16(address);
	idt[interrupt_no].high_offset = high_16(address);
	idt[interrupt_no].segment_selector = GDT_KERNEL_CODE_OFFSET;
	idt[interrupt_no].ist_offset = 0;
	idt[interrupt_no].flags = 0x8E;
}

void idt_load() {
	idt_ptr.limit = sizeof(idt_entry_t) * IDT_NO_OF_ENTRIES - 1;
	idt_ptr.base = (u32) &idt;
	
	__asm__ __volatile__("lidt (%0)" : : "r" (&idt_ptr));  // disable any compiler optimizations by marking it volatile
}