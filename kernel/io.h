#pragma once

unsigned char port_read_byte(unsigned short port);
unsigned short port_read_word(unsigned short port);

void port_write_byte(unsigned short port, unsigned char data);
void port_write_word(unsigned short port, unsigned char data);