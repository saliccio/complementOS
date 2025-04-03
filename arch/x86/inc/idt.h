#pragma once

#include <types.h>

#define IDT_NO_OF_ENTRIES 256
#define EXC_FLAGS 0x8E
#define IRQ_FLAGS 0x8E

// PIT interrupts are disabled, PIT works with polling
#define MASTER_IRQ_MASK (1 << INTVEC_TIMER)

// Interrupt descriptor table (IDT)

// Struct for an entry in IDT
typedef struct idt_entry
{
    u16_ct low_offset;       // Bits 0-15 of handler address
    u16_ct segment_selector; // Segment selector which must point a valid entry in GDT. Bits 0-1: Requested privilege
                             // level. Bit 2: If 0, GDT is used, if 1, LDT is used. Bits 3-15: Index of the referenced
                             // GDT or LDT entry.
    u8_ct ist_offset;        // Interrupt stack table offset. It won't be used, so leave it 0.

    /* Bit 7: Present bit. Must be 1.
    Bits 5-6: Privilege level needed to initiate the interrupt. 0 is kernel level, and 3 is user level.
    Bit 4: Must be 0.
    Bits 0-3: Determines the type. 0xE = 64-bit interrupt. 0xF = 64-bit trap. */
    u8_ct flags;
    u16_ct mid_offset;                 // Bits 16-31 of handler address
    u32_ct high_offset;                // Bits 32-63 of handler address
    u32_ct reserved;                   // Reserved, must be 0
} __attribute__((packed)) idt_entry_t; // Tell the compiler to use packing to avoid padding in the struct

// Struct for pointing the IDT to the CPU via 'LIDT' instruction
typedef struct idt_ptr
{
    u16_ct limit;                    // Size of the IDT - 1
    u64_ct base;                     // Starting address of the IDT
} __attribute__((packed)) idt_ptr_t; // Avoid padding

// Initializes IDT by setting up entries.
void idt_init();