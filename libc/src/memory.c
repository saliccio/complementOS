#include "libc/memory.h"

void mem_copy(const u8_ct *src_ptr, u8_ct *dest_ptr, u32_ct bytes)
{
    for (u32_ct i = 0; i < bytes; i++)
    {
        dest_ptr[i] = src_ptr[i];
    }
}

void mem_copy_str(const s8_ct *src_ptr, s8_ct *dest_ptr)
{
    int i = 0;
    char c;
    while ((c = src_ptr[i]) != '\0')
    {
        dest_ptr[i] = src_ptr[i];
        i++;
    }
    dest_ptr[i] = '\0';
}

void mem_set(u8_ct *dest_ptr, u8_ct data, u32_ct bytes)
{
    for (u32_ct i = 0; i < bytes; i++)
    {
        dest_ptr[i] = data;
    }
}

void swap(u8_ct *ptr1, u8_ct *ptr2, u32_ct bytes)
{
    u8_ct *temp_array1 = NULL;
    mem_copy(ptr1, temp_array1, bytes);
    mem_copy(ptr2, ptr1, bytes);
    mem_copy(temp_array1, ptr2, bytes);
}