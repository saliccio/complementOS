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
#include "paging.h"
#include "pit.h"
#include "stack.h"

static atomic_ct initialized_cpu_count = 0;
static spinlock_ct boot_spinlock;

__attribute__((section(".text.start"), used)) void boot_main()
{
    err_code_ct ret = NO_ERROR;
    u32_ct running_core_id;

    arch_atomic_increment(&initialized_cpu_count);

    if (initialized_cpu_count == 1) // If the current core is the BSP
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

    running_core_id = smp_get_core_id();

    smp_lock(&boot_spinlock);

    addr_ct stack_base = mem_area_alloc_with_alignment(KSTACK_SIZE, 0x1000);
    ret = mem_map(mem_get_kernel_mem_info(), stack_base - KSTACK_SIZE, KSTACK_SIZE / PAGE_SIZE, READ_WRITE);

    if (NO_ERROR != ret && ALREADY_MAPPED != ret)
    {
        vga_printf("An error occurred while mapping the stack of core %d!\n", running_core_id);
        while (1)
        {
        }
    }

    vga_printf("Core %d initialized.\n", running_core_id);

    smp_unlock(&boot_spinlock);

    ASM("sti"); // Enable interrupts again

    run_func_with_stack(stack_base, core_entry, NULL);
}