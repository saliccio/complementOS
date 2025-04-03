#include "core/init.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "kernelHeap.h"
#include "types.h"

void core_entry()
{
    call_static_hook_functions(CORE_INIT_START);

    bool_ct ret = kheap_init();
    if (!ret)
    {
        vga_print("An error occurred while initializing the kernel heap!\n");
        return;
    }

    call_static_hook_functions(CORE_INIT_END);
}