ENTRY(main)

OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)

KERNEL_VMA = 0xFFFFFFFF80000000;
KSTACK_SIZE = 32K;
KHEAP_SIZE = 64K;

SECTIONS
{
   . = 0x1000;
    
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

    kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
