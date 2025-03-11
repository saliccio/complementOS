#include "ipi.h"
#include "lapic.h"

#define INIT_IPI_ASSERT 0x000C4500   // INIT IPI (Level Assert, All-Excluding Self)
#define INIT_IPI_DEASSERT 0x000C0400 // INIT IPI (Level De-Assert, All-Excluding Self)
#define STARTUP_IPI 0x000C4600       // SIPI
#define AP_START_VECTOR 0x08         // AP start address is 0x8000

void ipi_send_init(u8_ct apic_id)
{
    // Write APIC ID to ICR high register
    lapic_write(LAPIC_ICR_HIGH, (u32_ct)apic_id << 24);

    // Send INIT IPI (assert)
    lapic_write(LAPIC_ICR_LOW, INIT_IPI_ASSERT); // INIT, Level-assert
    while (lapic_read(LAPIC_ICR_LOW) & (1 << 12))
    {
        // Wait for delivery
    }

    // Send INIT IPI (de-assert)
    lapic_write(LAPIC_ICR_LOW, INIT_IPI_DEASSERT); // INIT, Level-deassert
    while (lapic_read(LAPIC_ICR_LOW) & (1 << 12))
    {
        // Wait for delivery
    }
}

void ipi_send_sipi(u8_ct apic_id)
{
    lapic_write(LAPIC_ICR_HIGH, (u32_ct)apic_id << 24);
    lapic_write(LAPIC_ICR_LOW, STARTUP_IPI | AP_START_VECTOR); // SIPI with vector
    while (lapic_read(LAPIC_ICR_LOW) & (1 << 12))
    {
        // Wait for delivery
    }
}