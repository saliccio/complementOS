#include "lapic.h"
#include "arch/asm.h"
#include "pit.h"

#define IPI_INIT 0xC500
#define IPI_DEASSERT 0x8500
#define SIPI_INIT 0x608

#define LAPIC_BASE 0xFEE00000 // Common default LAPIC base address
#define LAPIC_ESR 0x280       // Error status register
#define LAPIC_ICR_LOW 0x300   // Interrupt Command Register 31:0
#define LAPIC_ICR_HIGH 0x310  // Interrupt Command Register 63:32
#define LAPIC_ID 0x20         // Local APIC ID register offset

static void lapic_write(u32_ct reg, u32_ct value)
{
    volatile u32_ct *addr = (volatile u32_ct *)(LAPIC_BASE + reg);
    *addr = value;
}

static u32_ct lapic_read(u32_ct reg)
{
    volatile u32_ct *addr = (volatile u32_ct *)(LAPIC_BASE + reg);
    return *addr;
}

static void lapic_wait_delivery()
{
    do
    {
        ASM("pause" : : : "memory");
    } while (lapic_read(LAPIC_ICR_LOW) & (1 << 12));
}

// Called from the BSP
void lapic_start_aps()
{
    for (u32_ct i = 0; i < CORE_COUNT; i++)
    {
        lapic_write(LAPIC_ESR, 0);                                                          // Reset ESR
        lapic_write(LAPIC_ICR_HIGH, (lapic_read(LAPIC_ICR_HIGH) & 0x00ffffff) | (i << 24)); // Select AP
        lapic_write(LAPIC_ICR_LOW, (lapic_read(LAPIC_ICR_LOW) & 0xfff00000) | IPI_INIT);    // Trigger INIT IPI
        lapic_wait_delivery();
        lapic_write(LAPIC_ICR_HIGH, (lapic_read(LAPIC_ICR_HIGH) & 0x00ffffff) | (i << 24)); // Select AP
        lapic_write(LAPIC_ICR_LOW, (lapic_read(LAPIC_ICR_LOW) & 0xfff00000) | IPI_DEASSERT);
        lapic_wait_delivery();
        pit_wait_ms(10);

        // send STARTUP IPI twice
        for (u32_ct j = 0; j < 2; j++)
        {
            lapic_write(LAPIC_ESR, 0);
            lapic_write(LAPIC_ICR_HIGH, (lapic_read(LAPIC_ICR_HIGH) & 0x00ffffff) | (i << 24));
            lapic_write(LAPIC_ICR_LOW, (lapic_read(LAPIC_ICR_LOW) & 0xfff0f800) | SIPI_INIT);
            pit_wait_ms(1);
            lapic_wait_delivery();
        }
    }
}