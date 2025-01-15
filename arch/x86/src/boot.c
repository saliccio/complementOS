#include "idt.h"
#include "archAsm.h"
#include "core/init.h"

/**
 * Defined in pmPrint.s
 */
extern void pm_print();

void boot_print(char* message)
{
    __asm__("mov %0, %%ebx" : : "r"(message) : "ebx");
    pm_print();
}

void boot_main()
{
    boot_print("Switched to 32-bit mode.");

    idt_init();

    ASM("sti");  // Enable interrupts again

    core_entry();
}