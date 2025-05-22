[bits 16]

save_elf_start_address:
    ; Check ELF magic number (0x7F 'E' 'L' 'F')
    mov si, ELF_START_ADDRESS
    cmp dword [si], 0x464C457F
    jne .bad_elf

    ret
    
    .bad_elf:
        mov bx, INVALID_ELF_ERROR
        call print
        jmp $

INVALID_ELF_ERROR: db "INV ELF", 0