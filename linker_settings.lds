ENTRY(main)

SECTIONS
{
   . = 0x1000;
   .text : { *(.text) *(.rodata) }
   . = 0x6000;
   .data : { *(.data) }
   . = 0x9000;
   .bss : { *(.bss) }
}
