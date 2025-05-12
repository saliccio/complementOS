#include "core/init.h"
#include "arch/asm.h"
#include "arch/atomics.h"
#include "arch/mmu.h"
#include "core/smp.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "idt.h"
#include "isr.h"
#include "lapic.h"
#include "paging.h"
#include "pit.h"

static atomic_ct initialized_cpu_count = 0;
static barrier_ct cpu_init_barrier = 0;
static spinlock_ct boot_spinlock;

__attribute__((section(".text.start"), used)) void boot_main()
{
    err_code_ct ret = NO_ERROR;

    arch_atomic_increment(&initialized_cpu_count);

    if (initialized_cpu_count == 1) // If the current core is the BSP
    {
        call_static_hook_functions(BOOT_START);

        idt_init();

        core_entry();

        ret = mmu_init();
        if (NO_ERROR != ret)
        {
            vga_print("An error occurred while initializing MMU!\n");
            while (1)
            {
            }
        }

        ret = lapic_init();
        if (NO_ERROR != ret)
        {
            vga_print("An error occurred while initializing LAPIC!\n");
            while (1)
            {
            }
        }

        smp_init_lock(&boot_spinlock);

        lapic_start_aps();

        call_static_hook_functions(BOOT_END);
    }
    else
    {
        idt_load();

        lapic_enable();
    }

    smp_lock(&boot_spinlock);
    vga_printf("Core %d initialized.\n", smp_get_core_id());
    smp_unlock(&boot_spinlock);

    /* Wait all cores to startup */
    smp_wait_on_barrier(&cpu_init_barrier, ALL_CORES_AFF);

    ASM("sti"); // Enable interrupts again

    while (1)
    {
        // Hang OS execution
    }
}