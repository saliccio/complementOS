#include "archAsm.h"
#include "core/init.h"
#include "idt.h"
#include "paging.h"

__attribute__((section(".text.start"), used)) void boot_main()
{
    idt_init();

    while (1)
    {
        /* Temporary barrier */
    }

    ASM("sti"); // Enable interrupts again

    char k = 'A';
    printf(&k);

    core_entry();
}