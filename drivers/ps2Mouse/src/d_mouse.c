#include "d_mouse.h"

// static int current_byte_index = 0;
// static mouse_packet_t current_packet;

void mouse_interrupt()
{
    // current_packet.bytes[current_byte_index] = PORT_READ_BYTE(MOUSE_DATA_PORT);
    // current_byte_index = (current_byte_index + 1) % MOUSE_PACKET_BYTES;
}

// mouse_packet_t mouse_get_packet() {

// }