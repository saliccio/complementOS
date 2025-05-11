#include "arch/atomics.h"
#include "arch/asm.h"

atomic_ct arch_atomic_load(atomic_ct *ptr)
{
    u64_ct value;
    ASM("movq %1, %0" : "=r"(value) : "m"(*ptr) : "memory");
    return value;
}

void arch_atomic_store(atomic_ct *ptr, atomic_ct value)
{
    ASM("xchgq %0, %1" : "=r"(value), "+m"(*ptr) : "0"(value) : "memory");
}

void arch_atomic_increment(atomic_ct *ptr)
{
    ASM("lock incq %0" : "+m"(*ptr) : : "memory");
}

void arch_atomic_decrement(atomic_ct *ptr)
{
    ASM("lock decq %0" : "+m"(*ptr) : : "memory");
}

bool_ct arch_atomic_cas(atomic_ct *ptr, atomic_ct old_val, atomic_ct new_val)
{
    s8_ct result;
    ASM("lock cmpxchgq %2, %1" : "=a"(result), "+m"(*ptr) : "r"(new_val), "0"(old_val) : "memory");
    return (bool_ct)result;
}

void arch_atomic_set_bit(atomic_ct *ptr, u32_ct bit)
{
    ASM("lock btsl %1, %0"
        : "+m"(*ptr) // memory operand (read-modify-write)
        : "Ir"(bit)  // bit index, immediate or register
        : "memory");
}

atomic_ct arch_atomic_add(atomic_ct *ptr, u64_ct val)
{
    u64_ct old;
    ASM("lock xadd %0, %1" : "=r"(old), "+m"(*ptr) : "0"(val) : "memory");
    return old;
}