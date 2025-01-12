#pragma once

#include <types.h>

#define IDT_NO_OF_ENTRIES 256

// Interrupt descriptor table (IDT)

// Struct for an entry in IDT
typedef struct {
	u16 low_offset;  // Lower 16 bits for the handler's address
	u16 segment_selector;  // Segment selector which must point a valid entry in GDT. Bits 0-1: Requested privilege level. Bit 2: If 0, GDT is used, if 1, LDT is used. Bits 3-15: Index of the referenced GDT or LDT entry.
	u8 ist_offset;  // Interrupt stack table offset. It won't be used, so leave it 0.
	
	/* Bit 7: Present bit. Must be 1.
	Bits 5-6: Privilege level needed to initiate the interrupt. 0 is kernel level, and 3 is user level.
	Bit 4: Must be 0.
	Bits 0-3: Determines the type. 0xE = 32-bit interrupt. 0xF = 32-bit trap. */
	u8 flags;
	u16 high_offset;  // Higher 16 bits for the handler's address
} __attribute__((packed)) idt_entry_t;  // Tell the compiler to use packing to avoid padding in the struct

// Struct for pointing the IDT to the CPU via 'LIDT' instruction
typedef struct {
	u16 limit;  // Size of the IDT - 1
	u32 base;  // Starting address of the IDT
} __attribute__((packed)) idt_ptr_t;  // Avoid padding

// Initializes IDT by setting up entries.
void idt_init();

// Loads the IDT into the CPU using 'LIDT' instruction.
void idt_load();

// Assign a routine address to given interrupt number.
void idt_assign_isr(int interrupt_no, u32 address);