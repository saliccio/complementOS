#include "core/kString.h"

void kmemset(u8_ct *dest_ptr, u8_ct data, u32_ct bytes)
{
    for (u32_ct i = 0; i < bytes; i++)
    {
        dest_ptr[i] = data;
    }
}