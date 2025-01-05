#include "idt.h"
#include "gdt.h"
#include <types.h>

idt_entry_t idt[IDT_NO_OF_ENTRIES];
idt_ptr_t idt_ptr;

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