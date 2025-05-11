#pragma once

#define ASM(...) __asm__ __volatile__(__VA_ARGS__)

#define PAUSE ASM("pause")