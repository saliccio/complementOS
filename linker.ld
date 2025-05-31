ENTRY(boot_main)

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

PAGE_SIZE = 4096;

KERNEL_VMA = 0xFFFFFF8000009000 + SIZEOF_HEADERS;
KERNEL_LMA = 0x9000 + SIZEOF_HEADERS;
VIRT_OFFSET = KERNEL_VMA - KERNEL_LMA;

KERNEL_AREA_SIZE = 10M;

PHDRS
{
  text         PT_LOAD FLAGS(5); /* R + X */
  data         PT_LOAD FLAGS(6); /* R + W */
  bss          PT_LOAD FLAGS(6); /* R + W */
  kernel_area  PT_NULL FLAGS(6); /* R + W */
}

SECTIONS
{
    . = KERNEL_VMA;
    
    .text : AT(ADDR(.text) - VIRT_OFFSET)
    {
        _code = .;
        *(.text.start)
        *(.text)
        *(.rodata*)
        _code_end = .;
    } :text

    .data ALIGN(PAGE_SIZE) : AT(ALIGN(ADDR(.data) - VIRT_OFFSET, PAGE_SIZE))
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

    .bss ALIGN(PAGE_SIZE): AT(ALIGN(ADDR(.bss) - VIRT_OFFSET, PAGE_SIZE))
    {
        _bss = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    } :bss

    .kernel_area ALIGN(PAGE_SIZE) : AT(ALIGN(ADDR(.kernel_area) - VIRT_OFFSET, PAGE_SIZE))
    {
        _kernel_area_start = .;
        . += KERNEL_AREA_SIZE;
        _kernel_area_end = .;

        _kernel_area_paddr_start = ALIGN(ADDR(.kernel_area) - VIRT_OFFSET, PAGE_SIZE);
        _kernel_area_paddr_end = _kernel_area_paddr_start + KERNEL_AREA_SIZE;
    } :kernel_area

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
