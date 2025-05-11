#pragma once

#include "types.h"

typedef volatile s64_ct atomic_ct;

atomic_ct arch_atomic_load(volatile atomic_ct *ptr);

void arch_atomic_store(volatile atomic_ct *ptr, atomic_ct value);

void arch_atomic_increment(volatile atomic_ct *ptr);

void arch_atomic_decrement(atomic_ct *ptr);

bool_ct arch_atomic_cas(atomic_ct *ptr, atomic_ct old_val, atomic_ct new_val);

void arch_atomic_set_bit(atomic_ct *ptr, u32_ct bit);

atomic_ct arch_atomic_add(atomic_ct *ptr, u64_ct val);