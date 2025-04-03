[bits 64]

global cpuid_is_supported_asm

section .text
cpuid_is_supported_asm:
    pushf                               ; Save EFLAGS
    pushf                               ; Store EFLAGS
    xor dword [rsp], 0x00200000         ; Invert the ID bit in stored EFLAGS
    popf                                ; Load stored EFLAGS (with ID bit inverted)
    pushf                               ; Store EFLAGS again (ID bit may or may not be inverted)
    pop rax                             ; rax = modified EFLAGS (ID bit may or may not be inverted)
    xor rax,[rsp]                       ; rax = whichever bits were changed
    popf                                ; Restore original EFLAGS
    and rax, 0x00200000                 ; rax = zero if ID bit can't be changed, else non-zero
    shr rax, 21                         ; Right-shift 21 times to make the return value 1 if rax is non-zero
    ret

cpuid_get_lapic_id_asm:
    mov rax, 1
    cpuid
    shr rbx, 24
    mov rax, rbx
    ret