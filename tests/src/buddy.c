#include "buddyAllocator.h"
#include "drivers/d_screen.h"
#include "kernelHeap.h"
#include "types.h"
#include "utils.h"

#define POOL_SIZE (BUDDY_MAX_BLOCK_SIZE)
#define SIZE_WITHOUT_HEADER(size) ((size) - sizeof(buddy_block_header_ct))

static buddy_pool_ct buddy_pool;

// Use kernel heap
static addr_ct memory_pool = &_kheap_start;

void test_buddy_1()
{
    TEST_START();

    bool_ct ret = buddy_init(&buddy_pool, memory_pool);
    TEST_ASSERT(ret, "ret=%d", ret);
    TEST_ASSERT(buddy_pool.total_used == 0, "total_used=%d", buddy_pool.total_used);

    TEST_PASS("Buddy system initialization handled correctly.");
}

void test_buddy_2()
{
    TEST_START();

    addr_ct ptr1 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(128));
    addr_ct ptr2 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(256));

    TEST_ASSERT(ptr1 != NULL, "ptr1 is %p", ptr1);
    TEST_ASSERT(ptr2 != NULL, "ptr2 is %p", ptr2);
    TEST_ASSERT(ptr1 != ptr2, "ptr1=%p, ptr2=%p", ptr1, ptr2);

    size_ct free_space_before = buddy_get_free_space(&buddy_pool);

    buddy_free(&buddy_pool, ptr1);
    buddy_free(&buddy_pool, ptr2);

    size_ct free_space_after = buddy_get_free_space(&buddy_pool);

    TEST_ASSERT(free_space_before + 128 + 256 == free_space_after, "free_space_before=%d, free_space_after=%d",
                free_space_before, free_space_after);
    TEST_ASSERT(free_space_after == POOL_SIZE, "free_space_after=%d", free_space_after);

    TEST_PASS("Buddy allocation and free handled correctly.");
}

void test_buddy_3()
{
    TEST_START();

    addr_ct ptr1 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(256));
    addr_ct ptr2 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(512));
    buddy_free(&buddy_pool, ptr1);                                     // Free 256-byte block
    addr_ct ptr3 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(128)); // Should use part of freed space

    TEST_ASSERT(ptr3 != NULL, "ptr3=%p", ptr3);
    TEST_ASSERT(ptr2 != ptr3, "ptr2=%p, ptr3=%p", ptr2, ptr3); // Should not overlap with ptr2
    buddy_free(&buddy_pool, ptr2);
    buddy_free(&buddy_pool, ptr3);

    TEST_PASS("Fragmentation case handled correctly.");
}

void test_buddy_4()
{
    TEST_START();

    addr_ct ptr1 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(512));
    addr_ct ptr2 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(512));

    TEST_ASSERT(ptr1 != NULL && ptr2 != NULL, "ptr1=%p, ptr2=%p", ptr1, ptr2);
    TEST_ASSERT(ptr2 == ptr1 + 512, "ptr1=%p, ptr2=%p", ptr1, ptr2); // Check if they are buddies

    buddy_free(&buddy_pool, ptr1);
    buddy_free(&buddy_pool, ptr2); // Should merge with ptr1

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    addr_ct ptr3 = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(1024));
    TEST_ASSERT(ptr3 == ptr1, "ptr1=%p, ptr3=%p", ptr1, ptr3); // Check if buddies are merged correctly
    buddy_free(&buddy_pool, ptr3);

    TEST_PASS("Buddy merging case handled correctly.");
}

void test_buddy_5()
{
    TEST_START();

    addr_ct ptr = buddy_alloc(&buddy_pool, POOL_SIZE * 2); // Too large
    TEST_ASSERT(ptr == NULL, "ptr=%p", ptr);

    TEST_PASS("Out-of-memory case handled correctly.");
}

void test_buddy_6()
{
    TEST_START();

    addr_ct ptr = buddy_alloc(&buddy_pool, SIZE_WITHOUT_HEADER(1));
    TEST_ASSERT(ptr != NULL, "ptr=%p", ptr);
    buddy_free(&buddy_pool, ptr);
    buddy_free(&buddy_pool, ptr); // Double-free

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    TEST_PASS("Double-free case handled correctly.");
}

void test_buddy_7()
{
    TEST_START();

    addr_ct ptr = NULL;
    buddy_free(&buddy_pool, ptr);
    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    TEST_PASS("Non-allocated free case handled correctly.");
}

void test_buddy_8()
{
    TEST_START();

    while (NULL != buddy_alloc(&buddy_pool, 1))
    {
        // Exhaust the pool
    }

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == 0, "free_space=%d", free_space);

    TEST_PASS("Memory exhaustion case handled correctly.");
}

static void (*test_funcs[])() = {test_buddy_1, test_buddy_2, test_buddy_3, test_buddy_4,
                                 test_buddy_5, test_buddy_6, test_buddy_7, test_buddy_8};

void test_buddy_main()
{
    for (u32_ct i = 0; i < 8; i++)
    {
        test_funcs[i]();
    }
}