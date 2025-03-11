#include "core/init.h"
#include "arch/asm.h"
#include "atomics.h"
#include "core/staticHooks.h"
#include "idt.h"
#include "ipi.h"
#include "lapic.h"
#include "paging.h"
#include "pit.h"

atomic_ct initialized_cpu_count = 0;

static void init_ap()
{
    for (u64_ct apic_id = 1; apic_id < CORE_COUNT; apic_id++)
    {
        ipi_send_init(apic_id);
        ipi_send_sipi(apic_id);
    }
}

__attribute__((section(".text.start"), used)) void boot_main()
{
    atomic_increment(&initialized_cpu_count);

    if (initialized_cpu_count == 1) // If the current core is the BSP
    {
        call_static_hook_functions(BOOT_START);

        pit_init();

        idt_init();

        core_entry();

        init_ap();

        call_static_hook_functions(BOOT_END);
    }

    ASM("sti"); // Enable interrupts again

    while (1)
    {
        // Hang OS execution
    }
}