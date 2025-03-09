#include "core/init.h"
#include "arch/asm.h"
#include "core/staticHooks.h"
#include "idt.h"
#include "paging.h"
#include "pit.h"
extern void _exc12();
__attribute__((section(".text.start"), used)) void boot_main()
{
    call_static_hook_functions(BOOT_START);

    vga_printf("%p\n", _exc12);

    pit_init();

    idt_init();

    ASM("sti"); // Enable interrupts again (all are masked though)

    core_entry();

    call_static_hook_functions(BOOT_END);

    while (1)
    {
        // Hang OS execution
    }
}