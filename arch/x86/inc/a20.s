[bits 16]

a20_enable:
    pusha

    mov     ax, 2403h  ; Check support
    int     15h
    jb      .a20_failed
    cmp     ah, 0
    jnz     .a20_failed

    mov     ax, 2402h  ; Check status
    int     15h
    jb      .a20_failed
    cmp     ah, 0
    jnz     .a20_failed

    cmp     al, 1  ; Check if activated
    jz      .a20_end

    mov     ax, 2401h  ; Activate
    int     15h
    jb      .a20_failed
    cmp     ah, 0
    jz      .a20_end
.a20_failed:
    mov bx, ACTIVATING_A20_FAILED
    call print
    jmp $
.a20_end:
    popa
    ret

ACTIVATING_A20_FAILED: db "A20 failed", 0