#include "lapic.h"
#include "arch/asm.h"

#define LAPIC_BASE 0xFEE00000 // Common default LAPIC base address

static inline void io_wait(void)
{
    ASM("outb %%al, $0x80" ::"a"(0));
}

void lapic_write(u32_ct reg, u32_ct value)
{
    volatile u32_ct *addr = (volatile u32_ct *)(LAPIC_BASE + reg);
    *addr = value;
    io_wait();
}

u32_ct lapic_read(u32_ct reg)
{
    volatile u32_ct *addr = (volatile u32_ct *)(LAPIC_BASE + reg);
    return *addr;
}