#include "drivers.h"
#include <cpu/isr.h>
#include <interrupts.h>
#include <memory/allocator.h>
#include <memory/info.h>
#include <array.h>

// Entry point of the kernel
int main() {
	idt_init();
	interrupt_setup_handlers();

	drivers_init();

	allocator_init();

	// printf("Welcome to complementOS!\n");

	return 0;
}