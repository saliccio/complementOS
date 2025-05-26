#pragma once

#include "types.h"

err_code_ct ahci_rw_main_disk(u64_ct start_lba, u32_ct count, addr_ct buffer, bool_ct is_write);