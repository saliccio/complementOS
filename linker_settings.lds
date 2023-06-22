ENTRY(main)

SECTIONS
{
   . = 0x1000;
   .text : { *(.text) *(.rodata) }
   . = 0x80000;
   .data : { *(.data) }
   .bss : { *(.bss) }
}
