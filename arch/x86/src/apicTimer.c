#include "apicTimer.h"
#include <ports.h>

void timer_init() {
    u32 divisor = HARDWARE_CLOCK_HZ / TIMER_HZ;
    port_write_byte(TIMER_COMMAND_PORT, TIMER_MODE);
    port_write_byte(TIMER_CHANNEL0_DATA_PORT, low_8(divisor));
    port_write_byte(TIMER_CHANNEL0_DATA_PORT, high_8(divisor));
}