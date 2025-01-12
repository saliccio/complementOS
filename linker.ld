ENTRY(main)

OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)

KERNEL_VMA = 0xFFFFFFFF80000000;
KSTACK_SIZE = 32K;
KHEAP_SIZE = 64K;

SECTIONS
{
    . = 0x1000;

    _kernel_start = .;
    
    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code = .;
        *(.text)
        *(.rodata*)
        . = ALIGN(4K);
        _code_end = .;
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA)
    {
        _data = .;
        *(.data)
        . = ALIGN(4K);
        _data_end = .;
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA)
    {
        _bss = .;
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
        _bss_end = .;
    }

    .kstack ALIGN(4K) : AT(ADDR(.kstack) - KERNEL_VMA)
    {
        _kstack_start = .;
        . += KSTACK_SIZE;
        _kstack_end = .;
    }

    .kheap ALIGN(4K) : AT(ADDR(.kheap) - KERNEL_VMA)
    {
        _kheap_start = .;
        . += KHEAP_SIZE;
        _kheap_end = .;
    }

    .hook_CORE_PRE_INIT : AT(ADDR(.hook_CORE_PRE_INIT) - KERNEL_VMA)
    {
        _hook_CORE_PRE_INIT_start = .;
        *SORT(.hook_CORE_PRE_INIT_*)
        _hook_CORE_PRE_INIT_end = .;
    }

    .hook_CORE_POST_INIT : AT(ADDR(.hook_CORE_POST_INIT) - KERNEL_VMA)
    {
        _hook_CORE_POST_INIT_start = .;
        *SORT(.hook_CORE_POST_INIT_*)
        _hook_CORE_POST_INIT_end = .;
    }

    _kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
