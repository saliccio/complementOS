#include "staticHooks.h"

static addr_ct func_addresses[NUM_OF_STATIC_HOOKS][2] =
{
    {&_hook_CORE_PRE_INIT_start, &_hook_CORE_PRE_INIT_end},
    {&_hook_CORE_POST_INIT_start, &_hook_CORE_POST_INIT_end}
};

err_code_ct call_static_hook_functions(static_hook_ct hook_point)
{
    err_code_ct ret = NO_ERROR;
    static_hook_0_arg_func_ct curr_address = func_addresses[hook_point][0];
    const static_hook_0_arg_func_ct end_address = func_addresses[hook_point][1];

    while (curr_address < end_address)
    {
        ret = curr_address();
        
        if (NO_ERROR != ret)
        {
            return ret;
        }

        curr_address++;
    }

    return ret;
}