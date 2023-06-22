gdt_start:
gdt_null:  ; the NULL descriptor
dd 0x0
dd 0x0
gdt_code:  ; code segment descriptor
dw 0xffff
dw 0x0
db 0x0
db 10011010b
db 11001111b
db 0x0
gdt_data:  ; data segment descriptor
dw 0xffff
dw 0x0
db 0x0
db 10010010b
db 11001111b
db 0x0
gdt_end:
gdt_descriptor:
dw gdt_end - gdt_start - 1  ; GDT size
dd gdt_start  ; GDT start address

CODE_SEG equ gdt_code - gdt_start  ; offset for the code segment (0x08)
DATA_SEG equ gdt_data - gdt_start  ; offset for the data segment (0x10)