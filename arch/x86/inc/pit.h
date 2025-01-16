#pragma once

#include <types.h>

#define HARDWARE_CLOCK_HZ 1193180
#define TIMER_HZ 100
#define TIMER_TICKS_PER_MILLISECOND TIMER_HZ * 1000

#define TIMER_COMMAND_PORT 0x43
#define TIMER_CHANNEL0_DATA_PORT 0x40
#define TIMER_CHANNEL1_DATA_PORT 0x41
#define TIMER_CHANNEL2_DATA_PORT 0x42

#define TIMER_MODE 0x36 // Channel: 0, Access mode: Both high and low bytes, Operating mode: Square wave generator

// Initializes the timer for given frequency value.
void pit_init();