#include "core/init.h"
#include "addrSpace.h"
#include "arch/bootInfo.h"
#include "bits.h"
#include "buddyAllocator.h"
#include "core/kernelHeap.h"
#include "core/ld.h"
#include "core/memArea.h"
#include "core/memDefs.h"
#include "core/staticHooks.h"
#include "drivers/ahci.h"
#include "drivers/d_screen.h"
#include "elfLoader.h"
#include "types.h"

static bool_ct init_mem()
{
    // Must init kheap first:
    addr_ct heap_start = ALIGN_UP_TO_ADDR(&_kernel_area_start, BUDDY_MAX_BLOCK_SIZE);
    vga_printf("Kernel Heap Start Address = %p\n", heap_start);

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

static err_code_ct load_entire_kernel()
{
    err_code_ct ret = NO_ERROR;
    addr_ct elf_start = (addr_ct)ELF_START_ADDR;
    size_ct already_loaded_size = (BOOT_LOADED_SECTORS)*SECTOR_SIZE;
    size_ct to_load_size = elf_get_load_size(elf_start);

    if (already_loaded_size < to_load_size)
    {
        u32_ct sector_count = (to_load_size - already_loaded_size) / SECTOR_SIZE;
        addr_ct load_addr = (addr_ct)ELF_START_ADDR + already_loaded_size;
        ret = ahci_rw_main_disk(NEXT_LBA_TO_LOAD, sector_count, load_addr, FALSE);
        if (NO_ERROR != ret)
        {
            return ret;
        }
    }

    ret = elf_load(elf_start, TRUE);
    if (NO_ERROR != ret)
    {
        return ret;
    }

    return NO_ERROR;
}

ATTACH_STATIC_HOOK(MMU_INIT_END, load_entire_kernel, 90);

void core_entry(addr_ct arg)
{
    while (1)
    {
        // Hang OS execution
    }
}