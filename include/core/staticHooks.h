#pragma once

#include "preprocessor.h"
#include "errCodes.h"

typedef enum static_hook
{
    CORE_PRE_INIT,
    CORE_POST_INIT,
    NUM_OF_STATIC_HOOKS
} static_hook_ct;

typedef err_code_ct (*static_hook_0_arg_func_ct)();
typedef static_hook_0_arg_func_ct CORE_PRE_INIT_func_ct;
typedef static_hook_0_arg_func_ct CORE_POST_INIT_func_ct;

#define ATTACH_STATIC_HOOK(hook_point, function, priority) \
    __attribute__((unused)) static CONCAT_2(hook_point, _func_ct) CONCAT_2(hook_point, _fptr) __attribute__((section(STRINGIFY(.hook_##hook_point##_##priority)))) = function;
