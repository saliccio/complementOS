#pragma once

#include "types.h"

#define SECTION(s) __attribute__((section(s)))

extern s8_ct _kernel_area_start;
extern s8_ct _kernel_area_end;

extern s8_ct _temp_elf_load_area_start;
extern s8_ct _temp_elf_load_area_end;