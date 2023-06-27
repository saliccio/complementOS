#include "drivers.h"
#include <cpu/isr.h>
#include <interrupts.h>

// Entry point of the kernel
int main() {
	idt_init();
	interrupt_setup_handlers();

	drivers_setup();
	
	printf("Welcome to complementOS!");

	return 0;
}