#include "pit.h"
#include "bits.h"
#include <arch/ports.h>

void pit_init()
{
    u32_ct divisor = HARDWARE_CLOCK_HZ / TIMER_HZ;
    port_write_byte(TIMER_COMMAND_PORT, TIMER_MODE);
    port_write_byte(TIMER_CHANNEL0_DATA_PORT, LOW_8(divisor));
    port_write_byte(TIMER_CHANNEL0_DATA_PORT, HIGH_8(divisor));
}