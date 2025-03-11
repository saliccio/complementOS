#include "atomics.h"
#include "arch/asm.h"

atomic_ct atomic_load(atomic_ct *ptr)
{
    return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

void atomic_store(atomic_ct *ptr, atomic_ct value)
{
    __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}

void atomic_increment(atomic_ct *ptr)
{
    ASM("lock incq %0" : "+m"(*ptr) : : "memory");
}

void atomic_decrement(atomic_ct *ptr)
{
    ASM("lock decq %0" : "+m"(*ptr) : : "memory");
}

bool_ct atomic_cas(atomic_ct *ptr, atomic_ct old_val, atomic_ct new_val)
{
    s8_ct result;
    ASM("lock cmpxchgq %2, %1" : "=a"(result), "+m"(*ptr) : "r"(new_val), "0"(old_val) : "memory");
    return (bool_ct)result;
}