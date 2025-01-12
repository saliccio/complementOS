#include <arch/idt.h>
#include "archAsm.h"

void boot_main()
{
    idt_init();

    ASM("sti");  // Enable interrupts again
}