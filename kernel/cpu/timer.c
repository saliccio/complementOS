#include "timer.h"
#include <cpu/io.h>

// static callback_hash_map_entry_t[]

void timer_init() {
    u32 divisor = HARDWARE_CLOCK_HZ / TIMER_TICKS_PER_SECOND;
    port_write_byte(TIMER_COMMAND_PORT, TIMER_MODE);
    port_write_byte(TIMER_CHANNEL0_DATA_PORT, low_8(divisor));
    port_write_byte(TIMER_CHANNEL0_DATA_PORT, high_8(divisor));
}

// void timer_run_after_milliseconds(void (*callback), int time) {

// }