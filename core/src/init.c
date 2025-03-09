#include "core/init.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"

void core_entry()
{
    call_static_hook_functions(CORE_INIT_START);

    // vga_printf("Welcome to complementOS!\n");

    call_static_hook_functions(CORE_INIT_END);
}