#include "drivers.h"
#include <cpu/isr.h>

// Entry point of the kernel
int main() {
	setup_drivers();
	
	printf("Welcome to complementOS!");

	return 0;
}