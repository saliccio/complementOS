[bits 64]

tss_start:  ; TSS structure begins here
    ; Reserved (must be zero)
    dd 0
    dd 0

    ; RSP0 (Stack pointer for privilege level 0)
    dq 0x0  ; Replace this with the actual kernel stack address

    ; RSP1 (Stack pointer for privilege level 1)
    dq 0x0  ; Typically unused in modern OSes

    ; RSP2 (Stack pointer for privilege level 2)
    dq 0x0  ; Typically unused in modern OSes

    ; Reserved
    dq 0x0

    ; IST (Interrupt Stack Table)
    dq 0x0  ; IST1 (e.g., Double Fault Stack) 
    dq 0x0  ; IST2
    dq 0x0  ; IST3
    dq 0x0  ; IST4
    dq 0x0  ; IST5
    dq 0x0  ; IST6
    dq 0x0  ; IST7

    ; Reserved
    dq 0x0

    ; IO Map Base Address (set to 0xFFFF to disable I/O map)
    dw 0x0
    dw 0x0
    dd 0xFFFF

tss_end:
