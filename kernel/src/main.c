#include "drivers.h"
#include <isr.h>
#include <interrupts.h>
#include <allocator.h>
#include <info.h>
#include <array.h>

// Entry point of the kernel
int main() {
	idt_init();
	interrupt_setup_handlers();

	drivers_init();

	//allocator_init();

	printf("Welcome to complementOS!\n");

	return 0;
}