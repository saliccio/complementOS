[bits 16]
get_memory_info:
    mov bp, 0
    mov ebx, 0
    mov di, MEMORY_MAP_BASE_ADDRESS
    .loop:
        mov ecx, 24
        mov edx, MEMORY_INFO_SIGNATURE
        mov eax, MEMORY_INFO_MAGIC_NUMBER
        int 0x15
        jc .failed
        cmp eax, MEMORY_INFO_SIGNATURE
        jne .return
        cmp ebx, 0
        je .return
        cmp ecx, 20
        jle .increase_index
        jmp .loop
    .increase_index:
        add di, 24
        inc bp
        jmp .loop
    .failed:
        mov bx, MEMORY_INFO_FETCH_FAILED
        call print
    .return:
        mov [MEMORY_MAP_LENGTH_ADDRESS], bp
        ret

MEMORY_MAP_LENGTH_ADDRESS equ 0x8000
MEMORY_MAP_BASE_ADDRESS equ 0x8004
MEMORY_INFO_MAGIC_NUMBER equ 0xE820
MEMORY_INFO_SIGNATURE equ 0x534D4150
MEMORY_INFO_FETCH_FAILED db "Failed to fetch the memory map.", 0