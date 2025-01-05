#pragma once

#define NULL 0

#define true 1
#define false 0

typedef          char  bool;

typedef unsigned int   u32;
typedef          int   s32;
typedef unsigned short u16;
typedef          short s16;
typedef unsigned char  u8;
typedef          char  s8;

#define low_8(data) (u8)((data) & 0xFF)
#define high_8(data) (u8)(((data) >> 8) & 0xFF)

#define low_16(data) (u16)((data) & 0xFFFF)
#define high_16(data) (u16)(((data) >> 16) & 0xFFFF)