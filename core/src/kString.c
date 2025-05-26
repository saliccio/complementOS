#include "core/kString.h"

void kmemset(u8_ct *dest_ptr, u8_ct data, u32_ct bytes)
{
    for (u32_ct i = 0; i < bytes; i++)
    {
        dest_ptr[i] = data;
    }
}

u32_ct kmemcmp(const addr_ct s1, const addr_ct s2, ulong_ct n)
{
    const u8_ct *p1 = s1;
    const u8_ct *p2 = s2;
    for (ulong_ct i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] - p2[i];
        }
    }

    return 0;
}

void kmemcpy(u8_ct *dst, const u8_ct *src, u64_ct size)
{
    for (u64_ct i = 0; i < size; i++)
    {
        dst[i] = src[i];
    }
}