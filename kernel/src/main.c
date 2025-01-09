#include "drivers.h"
#include <allocator.h>
#include <info.h>
#include <array.h>
#include <isr.h>

// Entry point of the kernel
int main() {
	idt_init();
	interrupts_enable();

	drivers_init();

	//allocator_init();

	printf("Welcome to complementOS!\n");

	return 0;
}