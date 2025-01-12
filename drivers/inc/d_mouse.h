#pragma once

#include <types.h>

#define MOUSE_DATA_PORT 0x60
#define MOUSE_CHECK_PORT 0x64
#define MOUSE_PACKET_BYTES 3

typedef struct {
    // bytes[0] = Bit 7: Y movement overflow, 6: X movement overflow, 5: Y movement sign bit, 4: X movement sign bit, 3: Always 1, 2: Middle button, 1: Right button, 0: Left button
    // bytes[1] = Horizontal delta movement - Movement values are around 1 or 2 for slow movements, and around 20 for very fast movements
    // bytes[2] = Vertical delta movement
    u8_ct bytes[MOUSE_PACKET_BYTES];
} __attribute__((packed)) mouse_packet_t;

// Called on mouse interrupt requests.
void mouse_interrupt();