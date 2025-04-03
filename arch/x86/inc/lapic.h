#pragma once

#include "types.h"

typedef enum ipi_destination
{
    NO_SHORTHAND = 0,
    SELF = 1,
    ALL_INCLUDING_SELF = 2,
    ALL_EXCLUDING_SELF = 3
} ipi_destination_ct;

void lapic_start_aps();