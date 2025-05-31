#include "core/addrSpace.h"
#include "drivers/d_screen.h"
#include "types.h"
#include "utils.h"

bool_ct test_addrspace_1()
{
    TEST_START();

    addr_ct virt_addr;
    err_code_ct ret = mem_map(mem_get_kernel_mem_info(), &virt_addr, 0x5000000, 1, PTE_READ_WRITE);
    TEST_ASSERT(NO_ERROR == ret, "ret=%d", ret);
    TEST_ASSERT(NULL != virt_addr, "virt_addr=%p", virt_addr);

    ret = mem_map_virt_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(0x5000000), 0x5000000, 1, PTE_READ_WRITE);
    TEST_ASSERT(NO_ERROR == ret, "ret=%d", ret);

    TEST_PASS("Happy path passed.");
}

bool_ct test_addrspace_2()
{
    TEST_START();

    err_code_ct ret = mem_map(mem_get_kernel_mem_info(), NULL, KERNELIZED_ADDR(0x6000000), 0, PTE_READ_WRITE);
    TEST_ASSERT(BAD_PARAM == ret, "ret=%d", ret);

    addr_ct virt_addr;
    ret = mem_map(mem_get_kernel_mem_info(), &virt_addr, KERNELIZED_ADDR(0x6000000), 0, PTE_READ_WRITE);
    TEST_ASSERT(BAD_PARAM == ret, "ret=%d", ret);
    TEST_ASSERT(NULL != virt_addr, "virt_addr=%p", virt_addr);

    ret = mem_map_virt_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(0x6000000), 0x6000000, 0, PTE_READ_WRITE);
    TEST_ASSERT(BAD_PARAM == ret, "ret=%d", ret);

    TEST_PASS("Invalid parameters test passed.");
}

bool_ct test_addrspace_3()
{
    TEST_START();

    addr_ct virt_addr;
    err_code_ct ret = mem_map(mem_get_kernel_mem_info(), &virt_addr, KERNELIZED_ADDR(0x5000000), 2, PTE_READ_WRITE);
    TEST_ASSERT(NO_ERROR == ret, "ret=%d", ret);
    TEST_ASSERT(NULL != virt_addr, "virt_addr=%p", virt_addr);

    ret = mem_map_virt_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(0x5000000), 0x5000000, 2, PTE_READ_WRITE);
    TEST_ASSERT(NO_MEMORY == ret, "ret=%d", ret);

    TEST_PASS("Already mapped test passed.");
}

bool_ct test_addrspace_4()
{
    TEST_START();

    addr_ct virt_addr;
    err_code_ct ret = mem_map(mem_get_kernel_mem_info(), &virt_addr, NULL, 2, PTE_READ_WRITE);
    TEST_ASSERT(NO_ERROR == ret, "ret=%d", ret);
    TEST_ASSERT(NULL != virt_addr, "virt_addr=%p", virt_addr);

    ret = mem_map_virt_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(0x5000000), 0x5000000, 2, PTE_READ_WRITE);
    TEST_ASSERT(NO_MEMORY == ret, "ret=%d", ret);

    TEST_PASS("NULL physical address test passed.");
}

bool_ct test_addrspace_5()
{
    TEST_START();

    addr_ct virt_addr;
    err_code_ct ret = mem_map(mem_get_kernel_mem_info(), &virt_addr, NULL, 2, FORCE_MAP);
    TEST_ASSERT(NO_ERROR == ret, "ret=%d", ret);
    TEST_ASSERT(NULL != virt_addr, "virt_addr=%p", virt_addr);

    ret = mem_map_virt_addr(mem_get_kernel_mem_info(), KERNELIZED_ADDR(0x5000000) - PAGE_SIZE, 0x5000000, 5, FORCE_MAP);
    TEST_ASSERT(NO_ERROR == ret, "ret=%d", ret);

    TEST_PASS("Force mapping test passed.");
}

static bool_ct (*test_funcs[])() = {test_addrspace_1, test_addrspace_2, test_addrspace_3, test_addrspace_4,
                                    test_addrspace_5};

void test_addrspace_main()
{
    for (u32_ct i = 0; i < 5; i++)
    {
        bool_ct ret = test_funcs[i]();
        if (!ret)
        {
            return;
        }
    }
}