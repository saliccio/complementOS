#pragma once

#define LOW_8(data) (u8_ct)((data) & 0xFF)
#define HIGH_8(data) (u8_ct)(((data) >> 8) & 0xFF)

#define LOW_16(data) (u16_ct)((data) & 0xFFFF)
#define HIGH_16(data) (u16_ct)(((data) >> 16) & 0xFFFF)