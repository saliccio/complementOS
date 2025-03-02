#include "arch/asm.h"
#include "core/init.h"
#include "idt.h"
#include "paging.h"

__attribute__((section(".text.start"), used)) void boot_main()
{
    idt_init();

    ASM("sti"); // Enable interrupts again (all are masked though)

    core_entry();

    while (1)
    {
        // Hang OS execution
    }
}