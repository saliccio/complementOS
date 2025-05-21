#pragma once

#include "asm.h"
#include "types.h"

// Reads 1 byte of data from given I/O port.
#define PORT_READ_BYTE(port)                                                                                           \
    ({                                                                                                                 \
        u8_ct data;                                                                                                    \
        ASM("in %%dx, %%al" : "=a"(data) : "d"(port));                                                                 \
        data;                                                                                                          \
    })

// Reads 2 bytes (a word) of data from given I/O port.
#define PORT_READ_WORD(port)                                                                                           \
    ({                                                                                                                 \
        u16_ct data;                                                                                                   \
        ASM("in %%dx, %%ax" : "=a"(data) : "d"(port));                                                                 \
        data;                                                                                                          \
    })

// Reads 4 bytes of data from given I/O port.
#define PORT_READ_4B(port)                                                                                             \
    ({                                                                                                                 \
        u32_ct data;                                                                                                   \
        ASM("in %%dx, %%eax" : "=a"(data) : "d"(port));                                                                \
        data;                                                                                                          \
    })

// Writes given 1 byte of data to given I/O port.
#define PORT_WRITE_BYTE(port, data) ({ ASM("out %%al, %%dx" : : "a"(data), "d"(port)); })

// Writes given 2 bytes (a word) of data to given I/O port.
#define PORT_WRITE_WORD(port, data) ({ ASM("out %%ax, %%dx" : : "a"(data), "d"(port)); })

// Writes given 4 bytes of data to given I/O port.
#define PORT_WRITE_4B(port, data) ({ ASM("out %%eax, %%dx" : : "a"(data), "d"(port)); })