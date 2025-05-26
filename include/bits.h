#pragma once

#define LOW_8(data) (u8_ct)((data)&0xFF)
#define HIGH_8(data) (u8_ct)(((data) >> 8) & 0xFF)

#define LOW_16(data) (u16_ct)((data)&0xFFFF)
#define HIGH_16(data) (u16_ct)(((data) >> 16) & 0xFFFF)

#define PTR_TO_SIZE(pointer) ((size_ct)(pointer))
#define IS_ADDR_ALIGNED(addr, alignment) (0 == (PTR_TO_SIZE(addr) & ((alignment)-1)))
#define ALIGN_DOWN_TO_ADDR(addr, alignment) ((addr_ct)((PTR_TO_SIZE(addr)) & ~((alignment)-1)))
#define ALIGN_DOWN_TO_VAL(val, alignment) ((val) & ~((alignment)-1))
#define ALIGN_UP_TO_ADDR(addr, alignment) ((addr_ct)((PTR_TO_SIZE(addr) + (alignment)-1) & ~((alignment)-1)))
#define ALIGN_UP_TO_VAL(val, alignment) ((val + (alignment)-1) & ~((alignment)-1))