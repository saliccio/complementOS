#include "idt.h"
#include "archAsm.h"
#include "core/init.h"

void boot_main()
{
    idt_init();

    ASM("sti");  // Enable interrupts again

    core_entry();
}