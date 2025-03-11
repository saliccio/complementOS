#pragma once

#include "types.h"

void ipi_send_init(u8_ct apic_id);

void ipi_send_sipi(u8_ct apic_id);