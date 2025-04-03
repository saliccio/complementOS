#include "arch/bits.h"

u32_ct arch_get_lsb(u32_ct value)
{
    if (value == 0)
        return -1; // No bits are set
    return __builtin_ctz(value);
}

u32_ct arch_get_msb(u32_ct value)
{
    if (value == 0)
        return -1; // No bits are set
    return 31 - __builtin_clz(value);
}