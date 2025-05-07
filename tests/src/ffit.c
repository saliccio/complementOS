#include "drivers/d_screen.h"
#include "ffAllocator.h"
#include "kernelHeap.h"
#include "types.h"
#include "utils.h"

#define POOL_SIZE (1024 * 128)
#define POOL_SIZE_SECONDARY (1024)

static firstfit_pool_ct ffit_pool;
static s8_ct *memory = (s8_ct *)0x10000;
static s8_ct *memory_secondary = (s8_ct *)0x50000;

bool_ct test_ffit_1()
{
    TEST_START();

    firstfit_init(&ffit_pool);

    bool_ct ret = firstfit_add_block(&ffit_pool, memory, POOL_SIZE);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("First-fit pool initialization passed.");
}

bool_ct test_ffit_2()
{
    TEST_START();

    addr_ct ptr1 = firstfit_alloc(&ffit_pool, 64);
    TEST_ASSERT(ptr1 != NULL, "ptr1=%p", ptr1);

    addr_ct ptr2 = firstfit_alloc(&ffit_pool, 128);
    TEST_ASSERT(ptr2 != NULL, "ptr2=%p", ptr2);

    bool_ct ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr2);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Basic allocation passed.");
}

bool_ct test_ffit_3()
{
    TEST_START();

    addr_ct ptr1 = firstfit_alloc(&ffit_pool, 32);
    addr_ct ptr2 = firstfit_alloc(&ffit_pool, 64);
    addr_ct ptr3 = firstfit_alloc(&ffit_pool, 128);

    TEST_ASSERT(NULL != ptr1 && NULL != ptr2 && NULL != ptr3, "ptr1=%p, ptr2=%p, ptr3=%p", ptr1, ptr2, ptr3);
    bool_ct ret = firstfit_free(&ffit_pool, ptr2); // Create a free space in the middle
    TEST_ASSERT(ret, "ret=%d", ret);

    addr_ct ptr4 = firstfit_alloc(&ffit_pool, 48);             // Should use the freed space
    TEST_ASSERT(ptr2 == ptr4, "ptr2=%p, ptr4=%p", ptr2, ptr4); // Should reuse the free block

    ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr3);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr4);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Fragmentation handling passed.");
}

bool_ct test_ffit_4()
{
    TEST_START();

    addr_ct ptr;
    firstfit_block_ct *block;
    while ((ptr = firstfit_alloc(&ffit_pool, 1024)) != NULL)
    {
        // Allocate until failure
    }

    // Restore the pool:
    firstfit_init(&ffit_pool);
    bool_ct ret = firstfit_add_block(&ffit_pool, memory, POOL_SIZE);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Memory exhaustion handled.");
}

bool_ct test_ffit_5()
{
    TEST_START();

    addr_ct base_addr = memory;
    addr_ct custom_start = base_addr + 1024; // Skip first 1 KB

    addr_ct ptr1 = firstfit_alloc_with_start_addr(&ffit_pool, 256, custom_start);
    TEST_ASSERT(ptr1 == custom_start, "ptr1=%p, custom_start=%p", ptr1,
                custom_start); // Should get exactly the requested address

    addr_ct ptr2 = firstfit_alloc_with_start_addr(&ffit_pool, 512, custom_start + 512);
    TEST_ASSERT(ptr2 == custom_start + 512, "ptr2=%p, custom_start=%p", ptr2,
                custom_start); // Should align correctly in the given range

    bool_ct ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ptr1=%p, ret=%d", ptr1, ret);

    ret = firstfit_free(&ffit_pool, ptr2);
    TEST_ASSERT(ret, "ptr2=%p, ret=%d", ptr2, ret);

    TEST_PASS("Allocation with start address 1 passed.");
}

bool_ct test_ffit_6()
{
    TEST_START();

    addr_ct base_addr = memory;
    addr_ct custom_start = base_addr + 1025;

    addr_ct ptr1 = firstfit_alloc_with_start_addr(&ffit_pool, 256, custom_start);
    TEST_ASSERT(ptr1 == custom_start, "ptr1=%p, custom_start=%p", ptr1,
                custom_start); // Should get exactly the requested address

    addr_ct ptr2 = firstfit_alloc_with_start_addr(&ffit_pool, 512, custom_start + 513);
    TEST_ASSERT(ptr2 == custom_start + 513, "ptr2=%p, custom_start=%p", ptr2,
                custom_start); // Should align correctly in the given range

    bool_ct ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr2);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Allocation with start address 2 passed.");
}

bool_ct test_ffit_7()
{
    TEST_START();

    addr_ct base_addr = memory;

    addr_ct ptr1 = firstfit_alloc_with_start_addr(&ffit_pool, 256, base_addr);
    TEST_ASSERT(ptr1 == base_addr, "ptr1=%p, base_addr=%p", ptr1,
                base_addr); // Should get exactly the requested address

    addr_ct ptr2 = firstfit_alloc_with_start_addr(&ffit_pool, 512, base_addr + 200);
    TEST_ASSERT(ptr2 == NULL, "ptr2=%p", ptr2); // Should be NULL

    bool_ct ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Allocation with start address 3 passed.");
}

bool_ct test_ffit_8()
{
    TEST_START();

    addr_ct base_addr = memory;

    addr_ct ptr1 = firstfit_alloc(&ffit_pool, 0);
    TEST_ASSERT(ptr1 == NULL, "ptr1=%p", ptr1);

    addr_ct ptr2 = firstfit_alloc_with_start_addr(&ffit_pool, 0, NULL);
    TEST_ASSERT(ptr2 == NULL, "ptr2=%p", ptr2);

    addr_ct ptr3 = firstfit_alloc_with_alignment(&ffit_pool, 0, 0);
    TEST_ASSERT(ptr3 == NULL, "ptr3=%p", ptr3);

    TEST_PASS("Invalid parameters test passed.");
}

bool_ct test_ffit_9()
{
    TEST_START();

    addr_ct ptr1 = firstfit_alloc_with_alignment(&ffit_pool, 128, 64); // 64-byte aligned
    TEST_ASSERT(ptr1 != NULL && *(size_ct *)&ptr1 % 64 == 0, "ptr1=%p", ptr1);

    addr_ct ptr2 = firstfit_alloc_with_alignment(&ffit_pool, 256, 128); // 128-byte aligned
    TEST_ASSERT(ptr2 != NULL && *(size_ct *)&ptr2 % 128 == 0, "ptr2=%p", ptr2);

    addr_ct ptr3 = firstfit_alloc_with_alignment(&ffit_pool, 128, 64); // 64-byte aligned
    TEST_ASSERT(ptr3 != NULL && *(size_ct *)&ptr3 % 64 == 0, "ptr3=%p", ptr3);

    bool_ct ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr2);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr3);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Allocation with alignment 1 passed.");
}

bool_ct test_ffit_10()
{
    TEST_START();

    addr_ct ptr1 = firstfit_alloc(&ffit_pool, 128);
    TEST_ASSERT(ptr1 != NULL, "ptr1=%p", ptr1);

    bool_ct ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(ret, "ret=%d", ret);

    ret = firstfit_free(&ffit_pool, ptr1);
    TEST_ASSERT(!ret, "ret=%d", ret);

    TEST_PASS("Double-free passed.");
}

bool_ct test_ffit_11()
{
    TEST_START();

    bool_ct ret = firstfit_add_block(&ffit_pool, memory, POOL_SIZE);
    TEST_ASSERT(!ret, "ret=%d", ret);

    ret = firstfit_add_block(&ffit_pool, memory - 1, POOL_SIZE);
    TEST_ASSERT(!ret, "ret=%d", ret);

    ret = firstfit_add_block(&ffit_pool, memory + POOL_SIZE - 1, POOL_SIZE);
    TEST_ASSERT(!ret, "ret=%d", ret); // Should overlap

    ret = firstfit_add_block(&ffit_pool, NULL, ~0);
    TEST_ASSERT(!ret, "ret=%d", ret); // Should overlap

    ret = firstfit_add_block(&ffit_pool, memory_secondary, POOL_SIZE_SECONDARY);
    TEST_ASSERT(ret, "ret=%d", ret);

    TEST_PASS("Block insertions passed.");
}

bool_ct test_ffit_12()
{
    TEST_START();

    bool_ct ret = firstfit_add_block(&ffit_pool, NULL, 8);
    TEST_ASSERT(ret, "ret=%d", ret);

    addr_ct ptr1 = firstfit_alloc_with_start_addr(&ffit_pool, 1, NULL);
    TEST_ASSERT(ptr1 == NULL, "ptr1=%p", ptr1);

    addr_ct ptr2 = firstfit_alloc_with_start_addr(&ffit_pool, 1, NULL + 1);
    TEST_ASSERT(ptr2 == NULL + 1, "ptr2=%p", ptr2);

    TEST_PASS("Allocation at addresses 0 and 1 passed.");
}

static bool_ct (*test_funcs[])() = {test_ffit_1, test_ffit_2, test_ffit_3, test_ffit_4,  test_ffit_5,  test_ffit_6,
                                    test_ffit_7, test_ffit_8, test_ffit_9, test_ffit_10, test_ffit_11, test_ffit_12};

void test_ffit_main()
{
    for (u32_ct i = 0; i < 12; i++)
    {
        bool_ct ret = test_funcs[i]();
        if (!ret)
        {
            return;
        }
    }
}