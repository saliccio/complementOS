#pragma once

#include "types.h"

u8_ct port_read_byte(u16_ct port);
u16_ct port_read_word(u16_ct port);

void port_write_byte(u16_ct port, u8_ct data);
void port_write_word(u16_ct port, u16_ct data);