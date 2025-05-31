#pragma once

#include "errCodes.h"
#include "preprocessor.h"

typedef enum static_hook
{
    BOOT_START,
    CORE_INIT_START,
    CORE_INIT_END,
    MMU_INIT_END,
    BOOT_END,
    NUM_OF_STATIC_HOOKS
} static_hook_ct;

typedef err_code_ct (*static_hook_0_arg_func_ct)();
typedef static_hook_0_arg_func_ct BOOT_START_func_ct;
typedef static_hook_0_arg_func_ct CORE_INIT_START_func_ct;
typedef static_hook_0_arg_func_ct CORE_INIT_END_func_ct;
typedef static_hook_0_arg_func_ct MMU_INIT_END_func_ct;
typedef static_hook_0_arg_func_ct BOOT_END_func_ct;

#define ATTACH_STATIC_HOOK(hook_point, function, priority)                                                             \
    __attribute__((unused)) static CONCAT_2(hook_point, _func_ct) CONCAT_3(hook_point, _fptr_, priority)               \
        __attribute__((section(STRINGIFY(.hook_##hook_point##_##priority)))) = function;

void call_static_hook_functions(static_hook_ct hook_point);