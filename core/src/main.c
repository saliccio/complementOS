#include "drivers.h"
#include <allocator.h>
#include <arch/isr.h>
#include <info.h>
#include <libc/array.h>

// Entry point of the kernel
int main()
{
    // idt_init();

    drivers_init();

    // allocator_init();

    printf("Welcome to complementOS!\n");

    return 0;
}