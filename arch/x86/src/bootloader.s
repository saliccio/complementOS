; First sector start

[bits 16]
[org 0x7c00]  ; bootloader memory offset

cli	 ; Temporarily disable interrupts until an IDT (interrupt descriptor table) for protected mode is set-up

mov ebp, [TEMP_STACK_BASE]
mov esp, ebp  ; Move the stack pointer to an appropriate place

call disk_load   ; Load sectors from the disk
call save_elf_start_address
call a20_enable  ; Enable A20 line of the memory bus
call switch_to_pm

[bits 16]
call 0x08:fill_page_tables  ; Initiate a FAR jump to ensure the pipeline flushes (to avoid problems that may arise when executing 16-bit instructions left in the pipeline)
jmp pm_start

[bits 16]
switch_to_pm:
    lgdt [gdt_pointer]	; Load GDT (Global Descriptor Table) into the CPU
    mov eax, cr0
    or eax, 1
    mov cr0, eax  ; Switch to the 32-bit protected mode
    ret

[bits 32]
pm_start:  ; Entry point for the protected mode
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax	; Make all segment registers point to the data segment description address (no actual segmentation will be used)

    jmp enable_long_mode

[bits 64]
call_main:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, [ELF_JUMP_ADDRESS_FIELD]
    call rax
    
%include "readDisk.s"
%include "longMode.s"
%include "print.s"
%include "a20.s"
%include "gdt.s"
%include "paging.s"
%include "elf.s"

AP_CODE_SEGMENT equ 0x800
KERNEL_PHYSICAL_ADDRESS_SEGMENT equ 0x900
ELF_START_ADDRESS equ 0x9000
ELF_JUMP_ADDRESS_FIELD equ 0x9018
TEMP_STACK_BASE dw 0x7000

times 510-($-$$) db 0  ; padding
dw	0xaa55  ; valid boot indicator

; Second sector start

[bits 16]
ap_start:
    cli

    mov ax, 0x280
    add [TEMP_STACK_BASE], ax
    mov ebp, [TEMP_STACK_BASE]
    mov esp, ebp

    lgdt [gdt_pointer]	; Load GDT (Global Descriptor Table) into the CPU
    mov eax, cr0
    or eax, 1
    mov cr0, eax  ; Switch to the 32-bit protected mode

    jmp 0x08:pm_start

times 512-($ - ap_start) db 0  ; padding