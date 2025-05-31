#include "core/staticHooks.h"
#include "suite.h"

void core_init_start_tests()
{
    // test_buddy_main();
}

void core_init_end_tests()
{
    // test_ffit_main();
}

void mmu_init_end_tests()
{
    test_addrspace_main();

    while (1)
    {
        // Suspend kernel execution
    }
}

ATTACH_STATIC_HOOK(CORE_INIT_START, core_init_start_tests, 90);
ATTACH_STATIC_HOOK(CORE_INIT_END, core_init_end_tests, 90);
ATTACH_STATIC_HOOK(MMU_INIT_END, mmu_init_end_tests, 90);