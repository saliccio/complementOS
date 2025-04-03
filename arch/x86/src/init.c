#include "core/init.h"
#include "arch/asm.h"
#include "atomics.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "idt.h"
#include "isr.h"
#include "lapic.h"
#include "paging.h"
#include "pit.h"

atomic_ct initialized_cpu_count = 0;

__attribute__((section(".text.start"), used)) void boot_main()
{
    atomic_increment(&initialized_cpu_count);

    if (initialized_cpu_count == 1) // If the current core is the BSP
    {
        call_static_hook_functions(BOOT_START);

        idt_init();

        core_entry();

        lapic_start_aps();

        call_static_hook_functions(BOOT_END);
    }

    ASM("sti"); // Enable interrupts again

    while (1)
    {
        // Hang OS execution
    }
}