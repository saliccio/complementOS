#pragma once

#include "arch/lapic.h"
#include "errCodes.h"
#include "types.h"

typedef enum ipi_destination
{
    NO_SHORTHAND = 0,
    SELF = 1,
    ALL_INCLUDING_SELF = 2,
    ALL_EXCLUDING_SELF = 3
} ipi_destination_ct;

err_code_ct lapic_init();

void lapic_start_aps();