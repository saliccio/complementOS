; First sector start

[bits 16]
[org 0x7c00]  ; bootloader memory offset

cli	 ; Temporarily disable interrupts until an IDT (interrupt descriptor table) for protected mode is set-up

mov ebp, [STACK_BASE]
mov esp, ebp  ; Move the stack pointer to an appropriate place
mov ax, 0x1000
add [STACK_BASE], ax  ; Move it upwards for the APs

call disk_load   ; Load sectors from the disk
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

    mov rax, [KERNEL_VIRTUAL_ADDRESS]
    jmp rax
    
%include "readDisk.s"
%include "longMode.s"
%include "print.s"
%include "a20.s"
%include "gdt.s"
%include "paging.s"

AP_CODE_SEGMENT equ 0x800
KERNEL_PHYSICAL_ADDRESS_SEGMENT equ 0x1000
KERNEL_VIRTUAL_ADDRESS dq 0xFFFFFF8000010000
STACK_BASE dw 0x99000

times 510-($-$$) db 0  ; padding
dw	0xaa55  ; valid boot indicator

; Second sector start

[bits 16]
ap_start:
    cli

    mov ebp, [STACK_BASE]
    mov esp, ebp
    mov ax, 0x1000
    add [STACK_BASE], ax

    lgdt [gdt_pointer]	; Load GDT (Global Descriptor Table) into the CPU
    mov eax, cr0
    or eax, 1
    mov cr0, eax  ; Switch to the 32-bit protected mode

    jmp 0x08:pm_start

times 512-($ - ap_start) db 0  ; padding