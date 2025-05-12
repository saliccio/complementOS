#include "idt.h"
#include "arch/asm.h"
#include "arch/ports.h"
#include "bits.h"
#include "gdt.h"
#include "pic.h"
#include <types.h>

static idt_entry_t idt[IDT_NO_OF_ENTRIES];
static idt_ptr_t idt_ptr;

static void idt_assign_isr(int interrupt_no, u64_ct address, u8_ct flags)
{
    idt[interrupt_no].low_offset = address & 0xFFFF;
    idt[interrupt_no].segment_selector = GDT_LONG_CODE_OFFSET;
    idt[interrupt_no].ist_offset = 0; // No special stack
    idt[interrupt_no].flags = flags;
    idt[interrupt_no].mid_offset = (address >> 16) & 0xFFFF;
    idt[interrupt_no].high_offset = (address >> 32) & 0xFFFFFFFF;
    idt[interrupt_no].reserved = 0;
}

void idt_load()
{
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_NO_OF_ENTRIES - 1;
    idt_ptr.base = (u64_ct)idt;

    ASM("lidt (%0)"
        :
        : "r"(&idt_ptr)); // disable any compiler optimizations by marking it
                          // volatile
}

void idt_init()
{
    idt_assign_isr(EXC0, (u64_ct)_exc0, EXC_FLAGS);
    idt_assign_isr(EXC1, (u64_ct)_exc1, EXC_FLAGS);
    idt_assign_isr(EXC2, (u64_ct)_exc2, EXC_FLAGS);
    idt_assign_isr(EXC3, (u64_ct)_exc3, EXC_FLAGS);
    idt_assign_isr(EXC4, (u64_ct)_exc4, EXC_FLAGS);
    idt_assign_isr(EXC5, (u64_ct)_exc5, EXC_FLAGS);
    idt_assign_isr(EXC6, (u64_ct)_exc6, EXC_FLAGS);
    idt_assign_isr(EXC7, (u64_ct)_exc7, EXC_FLAGS);
    idt_assign_isr(EXC8, (u64_ct)_exc8, EXC_FLAGS);
    idt_assign_isr(EXC9, (u64_ct)_exc9, EXC_FLAGS);
    idt_assign_isr(EXC10, (u64_ct)_exc10, EXC_FLAGS);
    idt_assign_isr(EXC11, (u64_ct)_exc11, EXC_FLAGS);
    idt_assign_isr(EXC12, (u64_ct)_exc12, EXC_FLAGS);
    idt_assign_isr(EXC13, (u64_ct)_exc13, EXC_FLAGS);
    idt_assign_isr(EXC14, (u64_ct)_exc14, EXC_FLAGS);
    idt_assign_isr(EXC15, (u64_ct)_exc15, EXC_FLAGS);
    idt_assign_isr(EXC16, (u64_ct)_exc16, EXC_FLAGS);
    idt_assign_isr(EXC17, (u64_ct)_exc17, EXC_FLAGS);
    idt_assign_isr(EXC18, (u64_ct)_exc18, EXC_FLAGS);
    idt_assign_isr(EXC19, (u64_ct)_exc19, EXC_FLAGS);
    idt_assign_isr(EXC20, (u64_ct)_exc20, EXC_FLAGS);
    idt_assign_isr(EXC21, (u64_ct)_exc21, EXC_FLAGS);
    idt_assign_isr(EXC22, (u64_ct)_exc22, EXC_FLAGS);
    idt_assign_isr(EXC23, (u64_ct)_exc23, EXC_FLAGS);
    idt_assign_isr(EXC24, (u64_ct)_exc24, EXC_FLAGS);
    idt_assign_isr(EXC25, (u64_ct)_exc25, EXC_FLAGS);
    idt_assign_isr(EXC26, (u64_ct)_exc26, EXC_FLAGS);
    idt_assign_isr(EXC27, (u64_ct)_exc27, EXC_FLAGS);
    idt_assign_isr(EXC28, (u64_ct)_exc28, EXC_FLAGS);
    idt_assign_isr(EXC29, (u64_ct)_exc29, EXC_FLAGS);
    idt_assign_isr(EXC30, (u64_ct)_exc30, EXC_FLAGS);
    idt_assign_isr(EXC31, (u64_ct)_exc31, EXC_FLAGS);

    idt_assign_isr(IRQ0, (u64_ct)_irq0, IRQ_FLAGS);
    idt_assign_isr(IRQ1, (u64_ct)_irq1, IRQ_FLAGS);
    idt_assign_isr(IRQ2, (u64_ct)_irq2, IRQ_FLAGS);
    idt_assign_isr(IRQ3, (u64_ct)_irq3, IRQ_FLAGS);
    idt_assign_isr(IRQ4, (u64_ct)_irq4, IRQ_FLAGS);
    idt_assign_isr(IRQ5, (u64_ct)_irq5, IRQ_FLAGS);
    idt_assign_isr(IRQ6, (u64_ct)_irq6, IRQ_FLAGS);
    idt_assign_isr(IRQ7, (u64_ct)_irq7, IRQ_FLAGS);
    idt_assign_isr(IRQ8, (u64_ct)_irq8, IRQ_FLAGS);
    idt_assign_isr(IRQ9, (u64_ct)_irq9, IRQ_FLAGS);
    idt_assign_isr(IRQ10, (u64_ct)_irq10, IRQ_FLAGS);
    idt_assign_isr(IRQ11, (u64_ct)_irq11, IRQ_FLAGS);
    idt_assign_isr(IRQ12, (u64_ct)_irq12, IRQ_FLAGS);
    idt_assign_isr(IRQ13, (u64_ct)_irq13, IRQ_FLAGS);
    idt_assign_isr(IRQ14, (u64_ct)_irq14, IRQ_FLAGS);
    idt_assign_isr(IRQ15, (u64_ct)_irq15, IRQ_FLAGS);

    // Remapping of Intel 8259 Programmable Interrupt Controller in cascade mode
    // (for IRQs 0-7, remap from 0x8-0xF to 0x20-0x27 to avoid conflicts with
    // ISRs) (for IRQs 8-15, remap from 0x70-0x77 to 0x28-0x2F to ensure
    // sequentiality)
    PORT_WRITE_BYTE(PIC_MASTER_COMMAND_PORT,
                    PIC_INIT_COMMAND); // Initialize master PIC
    PORT_WRITE_BYTE(PIC_SLAVE_COMMAND_PORT,
                    PIC_INIT_COMMAND); // Initialize slave PIC
    PORT_WRITE_BYTE(PIC_MASTER_DATA_PORT,
                    PIC_MASTER_VECTOR_OFFSET); // Remap IRQs 0-7
    PORT_WRITE_BYTE(PIC_SLAVE_DATA_PORT,
                    PIC_SLAVE_VECTOR_OFFSET); // Remap IRQs 8-15
    PORT_WRITE_BYTE(PIC_MASTER_DATA_PORT,
                    PIC_SLAVE_IRQ_NO); // Tell master PIC which IRQ input the
                                       // slave PIC resides at
    PORT_WRITE_BYTE(PIC_SLAVE_DATA_PORT,
                    PIC_SLAVE_CASCADE_IDENTITY); // Tell Slave PIC its cascade identity
    PORT_WRITE_BYTE(PIC_MASTER_DATA_PORT,
                    PIC_8086_MODE); // Make master PIC use 8086 mode
    PORT_WRITE_BYTE(PIC_SLAVE_DATA_PORT,
                    PIC_8086_MODE); // Make slave PIC use 8086 mode
    PORT_WRITE_BYTE(PIC_MASTER_DATA_PORT,
                    ~0x0); // Mask all interrupts for master PIC (disable)
    PORT_WRITE_BYTE(PIC_SLAVE_DATA_PORT,
                    ~0x0); // Mask all interrupts for slave PIC (disable)

    idt_load();
}