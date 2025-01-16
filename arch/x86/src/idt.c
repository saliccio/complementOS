#include "idt.h"
#include "arch/ports.h"
#include "archAsm.h"
#include "bits.h"
#include "gdt.h"
#include "pic.h"
#include <types.h>

static idt_entry_t idt[IDT_NO_OF_ENTRIES];
static idt_ptr_t idt_ptr;

void idt_init()
{
    idt_assign_isr(0, (u32_ct)_exc0);
    idt_assign_isr(1, (u32_ct)_exc1);
    idt_assign_isr(2, (u32_ct)_exc2);
    idt_assign_isr(3, (u32_ct)_exc3);
    idt_assign_isr(4, (u32_ct)_exc4);
    idt_assign_isr(5, (u32_ct)_exc5);
    idt_assign_isr(6, (u32_ct)_exc6);
    idt_assign_isr(7, (u32_ct)_exc7);
    idt_assign_isr(8, (u32_ct)_exc8);
    idt_assign_isr(9, (u32_ct)_exc9);
    idt_assign_isr(10, (u32_ct)_exc10);
    idt_assign_isr(11, (u32_ct)_exc11);
    idt_assign_isr(12, (u32_ct)_exc12);
    idt_assign_isr(13, (u32_ct)_exc13);
    idt_assign_isr(14, (u32_ct)_exc14);
    idt_assign_isr(15, (u32_ct)_exc15);
    idt_assign_isr(16, (u32_ct)_exc16);
    idt_assign_isr(17, (u32_ct)_exc17);
    idt_assign_isr(18, (u32_ct)_exc18);
    idt_assign_isr(19, (u32_ct)_exc19);
    idt_assign_isr(20, (u32_ct)_exc20);
    idt_assign_isr(21, (u32_ct)_exc21);
    idt_assign_isr(22, (u32_ct)_exc22);
    idt_assign_isr(23, (u32_ct)_exc23);
    idt_assign_isr(24, (u32_ct)_exc24);
    idt_assign_isr(25, (u32_ct)_exc25);
    idt_assign_isr(26, (u32_ct)_exc26);
    idt_assign_isr(27, (u32_ct)_exc27);
    idt_assign_isr(28, (u32_ct)_exc28);
    idt_assign_isr(29, (u32_ct)_exc29);
    idt_assign_isr(30, (u32_ct)_exc30);
    idt_assign_isr(31, (u32_ct)_exc31);

    // Remapping of Intel 8259 Programmable Interrupt Controller in cascade mode
    // (for IRQs 0-7, remap from 0x8-0xF to 0x20-0x27 to avoid conflicts with
    // ISRs) (for IRQs 8-15, remap from 0x70-0x77 to 0x28-0x2F to ensure
    // sequentiality)
    port_write_byte(PIC_MASTER_COMMAND_PORT,
                    PIC_INIT_COMMAND); // Initialize master PIC
    port_write_byte(PIC_SLAVE_COMMAND_PORT,
                    PIC_INIT_COMMAND); // Initialize slave PIC
    port_write_byte(PIC_MASTER_DATA_PORT,
                    PIC_MASTER_VECTOR_OFFSET); // Remap IRQs 0-7
    port_write_byte(PIC_SLAVE_DATA_PORT,
                    PIC_SLAVE_VECTOR_OFFSET); // Remap IRQs 8-15
    port_write_byte(PIC_MASTER_DATA_PORT,
                    PIC_SLAVE_IRQ_NO); // Tell master PIC which IRQ input the
                                       // slave PIC resides at
    port_write_byte(PIC_SLAVE_DATA_PORT,
                    PIC_SLAVE_CASCADE_IDENTITY); // Tell Slave PIC its cascade identity
    port_write_byte(PIC_MASTER_DATA_PORT,
                    PIC_8086_MODE); // Make master PIC use 8086 mode
    port_write_byte(PIC_SLAVE_DATA_PORT,
                    PIC_8086_MODE); // Make slave PIC use 8086 mode
    port_write_byte(PIC_MASTER_DATA_PORT,
                    0x0); // Reset interrupt masks for master PIC
    port_write_byte(PIC_SLAVE_DATA_PORT,
                    0x0); // Reset interrupt masks for slave PIC

    idt_assign_isr(32, (u32_ct)_irq0);
    idt_assign_isr(33, (u32_ct)_irq1);
    idt_assign_isr(34, (u32_ct)_irq2);
    idt_assign_isr(35, (u32_ct)_irq3);
    idt_assign_isr(36, (u32_ct)_irq4);
    idt_assign_isr(37, (u32_ct)_irq5);
    idt_assign_isr(38, (u32_ct)_irq6);
    idt_assign_isr(39, (u32_ct)_irq7);
    idt_assign_isr(40, (u32_ct)_irq8);
    idt_assign_isr(41, (u32_ct)_irq9);
    idt_assign_isr(42, (u32_ct)_irq10);
    idt_assign_isr(43, (u32_ct)_irq11);
    idt_assign_isr(44, (u32_ct)_irq12);
    idt_assign_isr(45, (u32_ct)_irq13);
    idt_assign_isr(46, (u32_ct)_irq14);
    idt_assign_isr(47, (u32_ct)_irq15);

    idt_load();
}

void idt_assign_isr(int interrupt_no, u32_ct address)
{
    idt[interrupt_no].low_offset = LOW_16(address);
    idt[interrupt_no].high_offset = HIGH_16(address);
    idt[interrupt_no].segment_selector = GDT_KERNEL_CODE_OFFSET;
    idt[interrupt_no].ist_offset = 0;
    idt[interrupt_no].flags = 0x8E;
}

void idt_load()
{
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_NO_OF_ENTRIES - 1;
    idt_ptr.base = (u32_ct)&idt;

    ASM("lidt (%0)"
        :
        : "r"(&idt_ptr)); // disable any compiler optimizations by marking it
                          // volatile
}