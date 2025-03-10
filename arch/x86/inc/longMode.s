[bits 32]

enable_long_mode:
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