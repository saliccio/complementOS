[bits 16]
[org 0x7c00]  ; bootloader memory offset

cli	 ; Temporarily disable interrupts until an IDT (interrupt descriptor table) for protected mode is set-up

mov ebp, STACK_BASE
mov esp, ebp  ; Move the stack pointer to an appropriate place

;call get_memory_info
[bits 16]  ; Ensure 16-bit real mode to make BIOS disk interrupt to load the kernel (0x13)
load_sectors:
    mov ax, [KERNEL_PHYSICAL_ADDRESS_SEGMENT]
    mov es, ax
    mov bx, 0
    call disk_load

call a20_enable  ; Enable A20 line of the memory bus

switch_to_pm:
    lgdt [gdt_pointer]	; Load GDT (Global Descriptor Table) into the CPU
    mov eax, cr0
    or eax, 1
    mov cr0, eax  ; Switch to the 32-bit protected mode

    jmp 0x08:pm_start  ; Initiate a FAR jump to ensure the pipeline flushes (to avoid problems that may arise when executing 16-bit instructions left in the pipeline)

[bits 32]  ; Ensure assembler to from now on, encode the instructions for 32-bit protected mode
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
    mov rax, [KERNEL_VIRTUAL_ADDRESS]
    jmp rax
    
%include "longMode.s"
%include "readDisk.s"
%include "print.s"
%include "printHex.s"
%include "a20.s"
%include "gdt.s"

KERNEL_PHYSICAL_ADDRESS_SEGMENT dw 0x1000
KERNEL_VIRTUAL_ADDRESS dq 0xFFFFFF8000010000

STACK_BASE equ 0x99900

times 510-($-$$)	db 0  ; padding
dw	0xaa55  ; valid boot indicator

; %include "getMemoryInfo.s"