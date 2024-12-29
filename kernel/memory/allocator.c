#include "allocator.h"
#include <memory/paging.h>
#include <memory/info.h>
#include <d_screen.h>

#define SEQUENTIAL_BUFFER_START 0x10000
#define NO_OF_MEMORY_BITMAPS 9