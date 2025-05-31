#include "core/smp.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "isr.h"
#include "lapic.h"

spinlock_ct ss;

static void decrementer_handler(isr_registers_ct *regs)
{
}

void decrementer_init()
{
    smp_init_lock(&ss);
    isr_set_handler(APIC_TIMER_VECTOR, decrementer_handler);
}