#include "core/init.h"
#include "addrSpace.h"
#include "buddyAllocator.h"
#include "core/kernelHeap.h"
#include "core/memArea.h"
#include "core/memDefs.h"
#include "core/staticHooks.h"
#include "drivers/d_screen.h"
#include "ld.h"
#include "types.h"

#define ALIGN_UP_TO(size, alignment) (((size) + (alignment)-1) & (~((alignment)-1)))

static bool_ct init_mem()
{
    // Must init kheap first:
    addr_ct heap_start = (addr_ct)ALIGN_UP_TO((size_ct)&_kernel_area_start, BUDDY_MAX_BLOCK_SIZE);
    vga_printf("heap_start=%p\n", heap_start);

    bool_ct ret = kheap_init(heap_start);
    if (!ret)
    {
        return FALSE;
    }

    ret = mem_area_init();
    if (!ret)
    {
        return FALSE;
    }

    heap_start = mem_area_alloc_with_start_addr(KHEAP_SIZE, heap_start);
    if (NULL == heap_start)
    {
        return FALSE;
    }

    ret = mem_init_kernel_addr_space();

    if (NO_ERROR == ret)
    {
        return TRUE;
    }

    return FALSE;
}

void core_init()
{
    call_static_hook_functions(CORE_INIT_START);

    bool_ct ret = init_mem();
    if (!ret)
    {
        vga_print("An error occurred while initializing the kernel memory!\n");
        while (1)
        {
        }
    }

    call_static_hook_functions(CORE_INIT_END);
}

void core_entry(addr_ct arg)
{
    while (1)
    {
        // Hang OS execution
    }
}