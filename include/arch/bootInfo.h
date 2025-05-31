#pragma once

#include "types.h"

#define SECTOR_SIZE 512
#define BOOT_LOADED_SECTORS 128
#define NEXT_LBA_TO_LOAD (BOOT_LOADED_SECTORS + 2)
#define ELF_START_ADDR ((addr_ct)0x9000)
#define IDENTITY_MAPPED_PAGES 1024