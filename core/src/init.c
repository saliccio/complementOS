#include "core/init.h"
#include "staticHooks.h"

void core_entry()
{
    call_static_hook_functions(CORE_PRE_INIT);

    printf("Welcome to complementOS!\n");
}