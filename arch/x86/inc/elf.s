[bits 16]

save_elf_start_address:
    ; Check ELF magic number (0x7F 'E' 'L' 'F')
    mov si, ELF_START_ADDRESS_PHYSICAL
    cmp dword [si], 0x464C457F
    jne .bad_elf

    ; Save pointer to entry point address (offset 0x18 in ELF header)
    add si, 0x18
    mov [KERNEL_ENTRY_POINT], si

    ret
    
    .bad_elf:
        mov bx, INVALID_ELF_ERROR
        call print
        jmp $

INVALID_ELF_ERROR: db "INV ELF", 0