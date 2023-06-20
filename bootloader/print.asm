print:
    pusha
print_start:
    mov al, [bx]
    cmp al, 0
    je print_done
    mov ah, 0x0e
    int 0x10
    add bx, 1
    jmp print_start
print_done:
    popa
    ret
print_nl:
    pusha
    mov ah, 0x0e
    mov al, 0x0a
    int 0x10
    mov al, 0x0d
    int 0x10
    popa
    ret