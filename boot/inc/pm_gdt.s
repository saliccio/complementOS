gdt_start:
    gdt_null:  ; the NULL descriptor
        dd 0x0
        dd 0x0
    gdt_kernel_code:  ; code segment descriptor for kernel
        dw 0xffff
        dw 0x0
        db 0x0
        db 10011010b
        db 11001111b
        db 0x0
    gdt_kernel_data:  ; data segment descriptor for kernel
        dw 0xffff
        dw 0x0
        db 0x0
        db 10010010b
        db 11001111b
        db 0x0
    gdt_user_code:  ; code segment descriptor for user
        dw 0xffff
        dw 0x0
        db 0x0
        db 11111010b
        db 11001111b
        db 0x0
    gdt_user_data:  ; data segment descriptor for user
        dw 0xffff
        dw 0x0
        db 0x0
        db 11110010b
        db 11001111b
        db 0x0
    gdt_end:
    gdt_descriptor:
        dw gdt_end - gdt_start - 1  ; GDT size
        dd gdt_start  ; GDT start address

CODE_SEG equ gdt_kernel_code - gdt_start  ; offset for the code segment (0x08)
DATA_SEG equ gdt_kernel_data - gdt_start  ; offset for the data segment (0x10)