ENTRY(boot_main)

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

KERNEL_VMA_START = 0xFFFFFF8000000000 + 0x9000 + SIZEOF_HEADERS;
KERNEL_AREA_SIZE = 10M;

PHDRS
{
  text PT_LOAD FLAGS(5); /* R + X */
  data PT_LOAD FLAGS(6); /* R + W */
}

SECTIONS
{
    . += KERNEL_VMA_START;

    _kernel_start = .;
    
    .text : AT(ADDR(.text) - KERNEL_VMA_START)
    {
        _code = .;
        *(.text.start)
        *(.text)
        *(.rodata*)
        _code_end = .;
    } :text

    .data : AT(ADDR(.data) - KERNEL_VMA_START)
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

        _hook_MMU_INIT_END_start = .;
        KEEP(SORT(*)(.hook_MMU_INIT_END_*))
        _hook_MMU_INIT_END_end = .;

        _hook_BOOT_END_start = .;
        KEEP(SORT(*)(.hook_BOOT_END_*))
        _hook_BOOT_END_end = .;

        _data_end = .;
    } :data

    .bss : AT(ADDR(.bss) - KERNEL_VMA_START)
    {
        _bss = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    } :data

    .kernel_area ALIGN(4K) : AT(ADDR(.kernel_area) - KERNEL_VMA_START)
    {
        _kernel_area_start = .;
        . += KERNEL_AREA_SIZE;
        _kernel_area_end = .;
    } :data

    _kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
