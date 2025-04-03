[bits 32]

set_page_table_entry:
    mov DWORD [edi], ebx
    add ebx, PAGE_SIZE
    add edi, 8
    dec ecx
    jnz set_page_table_entry
    ret

fill_page_tables:
    mov ax, 0x10
    mov es, ax

    mov edi, PML4_ADDRESS
    mov ecx, PAGE_TABLE_SIZES
    xor eax, eax
    rep stosd

    ; PML4[0] = PDPT, PDPT[0] = PAGE_DIR, PAGE_DIR[0] = PAGE_TABLE. Flags = 1: Present, 2: Read and write
    mov edi, PML4_ADDRESS
    mov DWORD [edi], (PDPT_ADDRESS) | PAGE_FLAGS
    add edi, PAGE_TABLE_SIZES
    mov DWORD [edi], (PAGE_DIRECTORY_ADDRESS) | PAGE_FLAGS
    add edi, PAGE_TABLE_SIZES
    mov DWORD [edi], (PAGE_TABLE_1_ADDRESS) | PAGE_FLAGS
    add edi, 8
    mov DWORD [edi], (PAGE_TABLE_2_ADDRESS) | PAGE_FLAGS
    mov edi, PAGE_TABLE_1_ADDRESS

    mov ebx, PAGE_FLAGS  ; Flags
    mov ecx, PAGE_TABLE_NO_OF_ENTRIES * 2
    call set_page_table_entry

    ; Map ffffff8000000000-ffffff8000400000 range to 0-400000 (first 4 MB)
    mov eax, (PDPT_ADDRESS | PAGE_FLAGS)
    mov DWORD [PML4_ADDRESS + (PAGE_TABLE_NO_OF_ENTRIES - 1) * 8], eax

    ret

PAGE_SIZE equ 0x1000
PAGE_TABLE_SIZES equ 0x1000
PAGE_TABLE_NO_OF_ENTRIES equ 512
PML4_ADDRESS equ 0x1000
PDPT_ADDRESS equ 0x2000
PAGE_DIRECTORY_ADDRESS equ 0x3000
PAGE_TABLE_1_ADDRESS equ 0x4000
PAGE_TABLE_2_ADDRESS equ 0x5000
PAGE_FLAGS equ 3