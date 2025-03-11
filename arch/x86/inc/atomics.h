#pragma once

#include "types.h"

typedef volatile s64_ct atomic_ct;

atomic_ct atomic_load(volatile atomic_ct *ptr);

void atomic_store(volatile atomic_ct *ptr, atomic_ct value);

void atomic_increment(volatile atomic_ct *ptr);

void atomic_decrement(atomic_ct *ptr);

bool_ct atomic_cas(atomic_ct *ptr, atomic_ct old_val, atomic_ct new_val);