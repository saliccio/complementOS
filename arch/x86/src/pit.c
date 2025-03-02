#include "pit.h"
#include "bits.h"
#include <arch/ports.h>

void pit_init()
{
    u32_ct divisor = HARDWARE_CLOCK_HZ / TIMER_HZ;
    PORT_WRITE_BYTE(TIMER_COMMAND_PORT, TIMER_MODE);
    PORT_WRITE_BYTE(TIMER_CHANNEL0_DATA_PORT, LOW_8(divisor));
    PORT_WRITE_BYTE(TIMER_CHANNEL0_DATA_PORT, HIGH_8(divisor));
}