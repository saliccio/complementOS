#include "core/init.h"
#include "arch/asm.h"
#include "arch/atomics.h"
#include "arch/mmu.h"
#include "core/addrSpace.h"
#include "core/memArea.h"
#include "core/memDefs.h"
#include "core/smp.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "idt.h"
#include "isr.h"
#include "lapic.h"
#include "pit.h"
#include "stack.h"

static atomic_ct next_cpu_id_to_initialize = 1;
static spinlock_ct boot_spinlock;

__attribute__((section(".text.start"), used)) void boot_main()
{
    err_code_ct ret = NO_ERROR;
    u32_ct running_core_id;

    arch_atomic_increment(&next_cpu_id_to_initialize);

    if (next_cpu_id_to_initialize == 2) // If the current core is the BSP
    {
        call_static_hook_functions(BOOT_START);

        idt_init();

        core_init();

        ret = mmu_init();
        if (NO_ERROR != ret)
        {
            vga_print("An error occurred while initializing MMU!\n");
            while (1)
            {
            }
        }

        (void)call_static_hook_functions(MMU_INIT_END);

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
    }
    else
    {
        idt_load();

        lapic_enable();
    }

    ASM("sti"); // Enable interrupts again

    running_core_id = smp_get_core_id();

    smp_lock(&boot_spinlock);
    if (0 == running_core_id)
    {
        call_static_hook_functions(BOOT_END);
    }
    addr_ct stack_base = mem_area_alloc_with_alignment(KSTACK_SIZE, 0x1000);
    smp_unlock(&boot_spinlock);

    run_func_with_stack(stack_base, core_entry, NULL);
}