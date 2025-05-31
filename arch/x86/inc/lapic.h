#pragma once

#include "arch/lapic.h"
#include "errCodes.h"
#include "types.h"

/* Timer interrupt vector */
#define APIC_TIMER_VECTOR 0x30

typedef enum ipi_destination
{
    NO_SHORTHAND = 0,
    SELF = 1,
    ALL_INCLUDING_SELF = 2,
    ALL_EXCLUDING_SELF = 3
} ipi_destination_ct;

err_code_ct lapic_init();
void lapic_enable();
void lapic_start_aps();
void lapic_eoi();