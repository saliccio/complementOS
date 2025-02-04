[bits 16]

print_hex:
    pusha
    mov cx, 0
.loop:
    cmp cx, 4
    je .return
    mov ax, dx
    and ax, 0x000f
    add al, 0x30
    cmp al, 0x39
    jle .step2
    add al, 7
.step2:
    mov bx, HEX_OUT + 5
    sub bx, cx
    mov [bx], al
    ror dx, 4
    add cx, 1
    jmp .loop
.return:
    mov bx, HEX_OUT
    call print
    popa
    ret

HEX_OUT:
    db '0x0000',0