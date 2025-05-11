#include "buddyAllocator.h"
#include "core/kernelHeap.h"
#include "drivers/d_screen.h"
#include "types.h"
#include "utils.h"

#define POOL_SIZE (BUDDY_MAX_BLOCK_SIZE)
#define SIZE_INCLUDING_HEADER(size) ((size) - sizeof(buddy_block_header_ct))
#define ALIGN_UP_TO(size, alignment) (((size) + (alignment)-1) & (~((alignment)-1)))

extern addr_ct _kernel_area_start;

static buddy_pool_ct buddy_pool;

bool_ct test_buddy_1()
{
    TEST_START();
    addr_ct heap_start = (addr_ct)ALIGN_UP_TO((size_ct)&_kernel_area_start, BUDDY_MAX_BLOCK_SIZE);
    bool_ct ret = buddy_init(&buddy_pool, heap_start);
    TEST_ASSERT(ret, "ret=%d", ret);
    TEST_ASSERT(buddy_pool.total_used == 0, "total_used=%d", buddy_pool.total_used);

    TEST_PASS("Buddy system initialization handled correctly.");
}

bool_ct test_buddy_2()
{
    TEST_START();

    addr_ct ptr1 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(128));
    addr_ct ptr2 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(256));

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

bool_ct test_buddy_3()
{
    TEST_START();

    addr_ct ptr1 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(256));
    TEST_ASSERT(ptr1 != NULL, "ptr1=%p", ptr1);

    addr_ct ptr2 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(512));
    TEST_ASSERT(ptr2 != NULL, "ptr2=%p", ptr2);

    buddy_free(&buddy_pool, ptr1); // Free 256-byte block

    addr_ct ptr3 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(128)); // Should use part of freed space
    TEST_ASSERT(ptr3 != NULL, "ptr3=%p", ptr3);

    TEST_ASSERT(ptr2 != ptr3, "ptr2=%p, ptr3=%p", ptr2, ptr3); // Should not overlap with ptr2
    buddy_free(&buddy_pool, ptr2);
    buddy_free(&buddy_pool, ptr3);

    TEST_PASS("Fragmentation case handled correctly.");
}

bool_ct test_buddy_4()
{
    TEST_START();

    addr_ct ptr1 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(512));
    addr_ct ptr2 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(512));

    TEST_ASSERT(ptr1 != NULL && ptr2 != NULL, "ptr1=%p, ptr2=%p", ptr1, ptr2);
    TEST_ASSERT(ptr2 == ptr1 + 512, "ptr1=%p, ptr2=%p", ptr1, ptr2); // Check if they are buddies

    buddy_free(&buddy_pool, ptr1);
    buddy_free(&buddy_pool, ptr2); // Should merge with ptr1

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    addr_ct ptr3 = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(1024));
    TEST_ASSERT(ptr3 == ptr1, "ptr1=%p, ptr3=%p", ptr1, ptr3); // Check if buddies are merged correctly
    buddy_free(&buddy_pool, ptr3);

    TEST_PASS("Buddy merging case handled correctly.");
}

bool_ct test_buddy_5()
{
    TEST_START();

    addr_ct ptr = buddy_alloc(&buddy_pool, POOL_SIZE * 2); // Too large
    TEST_ASSERT(ptr == NULL, "ptr=%p", ptr);

    TEST_PASS("Out-of-memory case handled correctly.");
}

bool_ct test_buddy_6()
{
    TEST_START();

    addr_ct ptr = buddy_alloc(&buddy_pool, SIZE_INCLUDING_HEADER(32));
    TEST_ASSERT(ptr != NULL, "ptr=%p", ptr);
    buddy_free(&buddy_pool, ptr);
    buddy_free(&buddy_pool, ptr); // Double-free

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    TEST_PASS("Double-free case handled correctly.");
}
bool_ct test_buddy_7()
{
    TEST_START();

    addr_ct ptr = NULL;
    buddy_free(&buddy_pool, ptr);
    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    TEST_PASS("Non-allocated free case handled correctly.");
}

bool_ct test_buddy_8()
{
    TEST_START();

    while (NULL != buddy_alloc(&buddy_pool, 1))
    {
        // Exhaust the pool
    }

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == 0, "free_space=%d", free_space);

    // Reset pool:
    bool_ct ret = buddy_init(&buddy_pool, buddy_pool.pool_start);
    TEST_ASSERT(ret, "ret=%d", ret);
    TEST_ASSERT(buddy_pool.total_used == 0, "total_used=%d", buddy_pool.total_used);

    TEST_PASS("Memory exhaustion case handled correctly.");
}

bool_ct test_buddy_9()
{
    TEST_START();

    while (NULL != buddy_alloc(&buddy_pool, 1))
    {
        // Exhaust the pool
    }

    size_ct free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == 0, "free_space=%d", free_space);

    addr_ct iter_addr = buddy_pool.pool_start + sizeof(buddy_block_header_ct);
    while (iter_addr < buddy_pool.pool_start + POOL_SIZE)
    {
        buddy_free(&buddy_pool, iter_addr);
        iter_addr += BUDDY_MIN_BLOCK_SIZE;
    }

    free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    // Make dummy frees:
    for (u32_ct i = 0; i < BUDDY_MIN_BLOCK_SIZE * 10; i += BUDDY_MIN_BLOCK_SIZE)
    {
        buddy_free(&buddy_pool, iter_addr + i);
    }

    free_space = buddy_get_free_space(&buddy_pool);
    TEST_ASSERT(free_space == POOL_SIZE, "free_space=%d", free_space);

    addr_ct alloc_addr1 = buddy_alloc(&buddy_pool, 1);
    TEST_ASSERT(alloc_addr1 != NULL, "alloc_addr1=%p", alloc_addr1);

    addr_ct alloc_addr2 = buddy_alloc(&buddy_pool, 1);
    TEST_ASSERT(alloc_addr2 != NULL, "alloc_addr2=%p", alloc_addr2);

    TEST_ASSERT(alloc_addr1 != alloc_addr2, "alloc_addr1=%p, alloc_addr2=%p", alloc_addr1, alloc_addr2);

    // Reset pool:
    bool_ct ret = buddy_init(&buddy_pool, buddy_pool.pool_start);
    TEST_ASSERT(ret, "ret=%d", ret);
    TEST_ASSERT(buddy_pool.total_used == 0, "total_used=%d", buddy_pool.total_used);

    TEST_PASS("Total merge from bottom to up case handled correctly.");
}

static bool_ct (*test_funcs[])() = {test_buddy_1, test_buddy_2, test_buddy_3, test_buddy_4, test_buddy_5,
                                    test_buddy_6, test_buddy_7, test_buddy_8, test_buddy_9};

void test_buddy_main()
{
    for (u32_ct i = 0; i < 9; i++)
    {
        bool_ct ret = test_funcs[i]();
        if (!ret)
        {
            return;
        }
    }
}