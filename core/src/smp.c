#include "core/smp.h"
#include "arch/asm.h"
#include "arch/atomics.h"
#include "arch/lapic.h"

u32_ct smp_get_core_id()
{
    return lapic_get_core_id();
}

void smp_wait_on_barrier(barrier_ct *barrier, affinity_ct affinity)
{
    arch_atomic_set_bit((barrier_ct)barrier, smp_get_core_id());
    while (arch_atomic_load((atomic_ct)barrier) != affinity)
    {
    }
}

void smp_init_lock(spinlock_ct *spinlock)
{
    spinlock->now_serving = 0;
    spinlock->next_ticket = 0;
}

void smp_lock(spinlock_ct *spinlock)
{
    atomic_ct ticket = arch_atomic_add(&spinlock->next_ticket, 1);

    while (arch_atomic_load(&spinlock->now_serving) != ticket)
    {
        PAUSE;
    }
}

void smp_unlock(spinlock_ct *spinlock)
{
    arch_atomic_increment(&spinlock->now_serving);
}