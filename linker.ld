ENTRY(boot_main)

OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386:x86-64)

KERNEL_VMA = 0xFFFFFF8000000000;
KSTACK_SIZE = 32K;
KHEAP_SIZE = 64K;

SECTIONS
{
    . = 0x8000;

    . += KERNEL_VMA;

    _kernel_start = .;
    
    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code = .;
        *(.text.start)
        *(.text)
        *(.rodata*)
        . = ALIGN(4K);
        _code_end = .;
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA)
    {
        _data = .;
        *(.data)

        _hook_CORE_PRE_INIT_start = .;
        KEEP(SORT(*)(.hook_CORE_PRE_INIT_*))
        _hook_CORE_PRE_INIT_end = .;

        _hook_CORE_POST_INIT_start = .;
        KEEP(SORT(*)(.hook_CORE_POST_INIT_*))
        _hook_CORE_POST_INIT_end = .;

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

    _kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
