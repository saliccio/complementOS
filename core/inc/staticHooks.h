#pragma once

#include "types.h"
#include "errCodes.h"
#include "core/staticHooks.h"

extern addr_ct _hook_CORE_PRE_INIT_start;
extern addr_ct _hook_CORE_PRE_INIT_end;

extern addr_ct _hook_CORE_POST_INIT_start;
extern addr_ct _hook_CORE_POST_INIT_end;

err_code_ct call_static_hook_functions(static_hook_ct hook_point);