#include "core/init.h"
#include "arch/asm.h"
#include "core/staticHooks.h"
#include "idt.h"
#include "paging.h"
#include "pit.h"

__attribute__((section(".text.start"), used)) void boot_main()
{
    call_static_hook_functions(BOOT_START);

    pit_init();

    idt_init();

    ASM("sti"); // Enable interrupts again

    core_entry();

    call_static_hook_functions(BOOT_END);

    while (1)
    {
        // Hang OS execution
    }
}