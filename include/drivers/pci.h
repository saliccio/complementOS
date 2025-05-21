#pragma once

#include "types.h"

u32_ct pci_config_read(u8_ct bus, u8_ct slot, u8_ct func, u8_ct offset);