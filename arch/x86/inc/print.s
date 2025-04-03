[bits 16]

print:
    pusha
    .start:
        mov al, [bx]
        cmp al, 0
        je print_end
        mov ah, 0x0e
        int 0x10
        add bx, 1
        jmp .start
print_nl:
    pusha
    mov ah, 0x0e
    mov al, 0x0a
    int 0x10
    mov al, 0x0d
    int 0x10
print_end:
    popa
    ret