#include "stack.h"
#include "arch/asm.h"

void run_func_with_stack(addr_ct stack_base, void (*func)(addr_ct), addr_ct arg)
{
    ASM("movq (%%rsp), %%rax\n" // Load return address from old stack into rax
        "movq %0, %%rsp\n"      // Switch to new stack
        :
        : "r"(stack_base)
        : "rax", "memory");

    func(arg);
}