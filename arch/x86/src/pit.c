#include "arch/ports.h"
#include "bits.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "isr.h"

#define PIT_FREQ 1193180
#define TICK_FREQ 100

#define TIMER_CHANNEL0_DATA_PORT 0x40
#define TIMER_CHANNEL1_DATA_PORT 0x41
#define TIMER_CHANNEL2_DATA_PORT 0x42
#define TIMER_COMMAND_PORT 0x43

#define TIMER_MODE 0x34 // Channel: 0, Access mode: Both high and low bytes, Operating mode: Programmable one-shot
#define TIMER_READ_BACK_COMMAND 0xE2
#define STATUS_BYTE_FINISHED (1 << 7)

void pit_wait_ms(u64_ct ms)
{
    u32_ct divisor = PIT_FREQ / TICK_FREQ; // Convert ms to PIT ticks

    // Set PIT Mode 1 (One-Shot) and load count
    PORT_WRITE_BYTE(TIMER_COMMAND_PORT, TIMER_MODE);
    PORT_WRITE_BYTE(TIMER_CHANNEL0_DATA_PORT, LOW_8(divisor));
    PORT_WRITE_BYTE(TIMER_CHANNEL0_DATA_PORT, HIGH_8(divisor));

    u16_ct prev_count = 0xFFFF, curr_count = 0xFFFF;
    u32_ct ticks = 0;
    while (ticks < (u32_ct)TICK_FREQ * ms / 1000)
    {
        do
        {
            // Send latch command to freeze the current counter value
            PORT_WRITE_BYTE(TIMER_COMMAND_PORT, 0x00);

            prev_count = curr_count;

            // Read the counter value (low byte first, then high byte)
            curr_count = PORT_READ_BYTE(TIMER_CHANNEL0_DATA_PORT);
            curr_count |= (PORT_READ_BYTE(TIMER_CHANNEL0_DATA_PORT) << 8);
        } while (curr_count <= prev_count);
        ticks++;
    }
}