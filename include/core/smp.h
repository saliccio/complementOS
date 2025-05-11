#pragma once

#include "arch/atomics.h"
#include "types.h"

#define ALL_CORES_AFF ((1 << CORE_COUNT) - 1)
#define GET_CORE_AFF(core) ((affinity_ct)(1 << core))

typedef struct spinlock
{
    atomic_ct now_serving;
    atomic_ct next_ticket;
} spinlock_ct;

typedef u64_ct barrier_ct;
typedef u64_ct affinity_ct;

u32_ct smp_get_core_id();
void smp_wait_on_barrier(barrier_ct *barrier, affinity_ct affinity);
void smp_init_lock(spinlock_ct *spinlock);
void smp_lock(spinlock_ct *spinlock);
void smp_unlock(spinlock_ct *spinlock);