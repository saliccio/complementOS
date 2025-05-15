#pragma once

#include "types.h"

void run_func_with_stack(addr_ct stack_base, void (*func)(addr_ct), addr_ct arg);