#pragma once

#include "types.h"

#define LAPIC_ICR_LOW 0x300
#define LAPIC_ICR_HIGH 0x310

void lapic_write(u32_ct reg, u32_ct value);

u32_ct lapic_read(u32_ct reg);