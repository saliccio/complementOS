#include "drivers.h"

// Entry point of the kernel
int main() {
	setup_drivers();
	
	print("Welcome to complementOS!");

	return 0;
}