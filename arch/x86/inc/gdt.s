[bits 64]

gdt_start:  ; GDT begins here

    ; Null Descriptor (0x00)
    dq 0x0000000000000000  ; Null segment (must be zero)

    ; Code Segment Descriptor for Protected Mode (0x08)
    dq 0x00cf9a000000ffff  ; Base = 0x0, Limit = 4 GiB, Code, Executable, Readable

    ; Data Segment Descriptor for Protected Mode (0x10)
    dq 0x00cf92000000ffff  ; Base = 0x0, Limit = 4 GiB, Data, Read/Write

    ; Code Segment Descriptor for Long Mode (64-bit) (0x18)
    dq 0x00af9a0000000000  ; Base = 0x0, Limit ignored, Code, Executable, 64-bit

    ; Data Segment Descriptor for Long Mode (64-bit) (0x20)
    dq 0x00af920000000000  ; Base = 0x0, Limit ignored, Data, Read/Write

gdt_end:  ; GDT ends here

gdt_pointer:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dq gdt_start                ; Address of GDT
