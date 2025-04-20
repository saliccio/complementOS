#pragma once

#define TEST_PRINT(message) (vga_print(message))
#define TEST_PRINTF(message, ...) (vga_printf(message, __VA_ARGS__))

#define TEST_START()                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        TEST_PRINTF("[TEST STARTED] FILE: %s | LINE: %d | Case: %s\n", __FILE__, __LINE__, __func__);                  \
    } while (0);

#define TEST_ASSERT(predicate, message, ...)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(predicate))                                                                                              \
        {                                                                                                              \
            TEST_PRINTF("[TEST FAILED] FILE: %s | LINE: %d | Message: ", __FILE__, __LINE__);                          \
            TEST_PRINTF((message), __VA_ARGS__);                                                                       \
            TEST_PRINT("\n");                                                                                          \
            return FALSE;                                                                                              \
        }                                                                                                              \
    } while (0);

#define TEST_PASS(message)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        TEST_PRINTF("[TEST PASSED] FILE: %s | LINE: %d | Message: %s\n", __FILE__, __LINE__, (message));               \
        return TRUE;                                                                                                   \
    } while (0);