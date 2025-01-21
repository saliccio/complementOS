[bits 32]

enable_long_mode:
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
    mov DWORD [edi], (PAGE_TABLE_ADDRESS) | PAGE_FLAGS
    add edi, PAGE_TABLE_SIZES

    mov ebx, PAGE_FLAGS  ; Flags
    mov ecx, PAGE_TABLE_NO_OF_ENTRIES
    .set_page_table_entry:
        mov DWORD [edi], ebx
        add ebx, PAGE_SIZE
        add edi, 8
        loop .set_page_table_entry

    ; Map ffffff8000000000-ffffff8000200000 range to 0-200000 (first 2 MB)
    mov eax, (PDPT_ADDRESS | PAGE_FLAGS)
    mov DWORD [PML4_ADDRESS + (PAGE_TABLE_NO_OF_ENTRIES - 1) * 8], eax

    ; Set CR4.PAE (physical address extension)
    mov edx, cr4
    or  edx, (1 << 5)
    mov cr4, edx

    ; Set EFER.LM (long mode)
    mov ecx, 0xC0000080
    rdmsr
    or eax, (1 << 8)
    wrmsr

    ; Move PML4 table address to CR3.
    mov eax, (PML4_ADDRESS | PAGE_FLAGS)
    mov cr3, eax

    ; Enable paging
    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

    jmp 0x18:call_main

PAGE_SIZE equ 0x1000
PAGE_TABLE_SIZES equ 0x1000
PAGE_TABLE_NO_OF_ENTRIES equ 512
PML4_ADDRESS equ 0x1000
PDPT_ADDRESS equ 0x2000
PAGE_DIRECTORY_ADDRESS equ 0x3000
PAGE_TABLE_ADDRESS equ 0x4000
PAGE_FLAGS equ 3