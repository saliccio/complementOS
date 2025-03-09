ENTRY(boot_main)

OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386:x86-64)

KERNEL_VMA = 0xFFFFFF8000000000;
KSTACK_SIZE = 32K;
KHEAP_SIZE = 64K;

SECTIONS
{
    . = 0x10000;

    . += KERNEL_VMA;

    _kernel_start = .;
    
    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code = .;
        *(.text.start)
        *(.text)
        *(.rodata*)
        _code_end = .;
    }

    .data : AT(ADDR(.data) - KERNEL_VMA)
    {
        _data = .;
        *(.data)

        _hook_BOOT_START_start = .;
        KEEP(SORT(*)(.hook_BOOT_START_*))
        _hook_BOOT_START_end = .;

        _hook_CORE_INIT_START_start = .;
        KEEP(SORT(*)(.hook_CORE_INIT_START_*))
        _hook_CORE_INIT_START_end = .;

        _hook_CORE_INIT_END_start = .;
        KEEP(SORT(*)(.hook_CORE_INIT_END_*))
        _hook_CORE_INIT_END_end = .;

        _hook_BOOT_END_start = .;
        KEEP(SORT(*)(.hook_BOOT_END_*))
        _hook_BOOT_END_end = .;

        _data_end = .;
    }

    .bss : AT(ADDR(.bss) - KERNEL_VMA)
    {
        _bss = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    }

    .kstack : AT(ADDR(.kstack) - KERNEL_VMA)
    {
        _kstack_start = .;
        . += KSTACK_SIZE;
        _kstack_end = .;
    }

    .kheap : AT(ADDR(.kheap) - KERNEL_VMA)
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
