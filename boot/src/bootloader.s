[org 0x7c00]  ; bootloader memory offset

mov ebp, STACK_BASE
mov esp, ebp  ; Move the stack pointer to an appropriate place

;call get_memory_info

call load_sectors

lgdt [gdt_descriptor]	; Load GDT (Global Descriptor Table) into the CPU
mov eax, cr0
or eax, 1
mov cr0, eax  ; Switch to the 32-bit protected mode
jmp CODE_SEG:pm_start  ; Initiate a FAR jump to ensure the pipeline flushes (to avoid problems that may arise when executing 16-bit instructions left in the pipeline)

[bits 16]  ; Ensure 16-bit real mode to make BIOS disk interrupt to load the kernel (0x13)
load_sectors:
    mov bx, [KERNEL_OFFSET]  ; Where to load in memory
    mov dh, 50  ; Sector count
    mov dl, [BOOTING_DRIVE]  ; Drive to boot from
    call disk_load

    ret

[bits 32]  ; Ensure assembler to from now on, encode the instructions for 32-bit protected mode
pm_start:  ; Entry point for the protected mode
    cli	 ; Temporarily disable interrupts until an IDT (interrupt descriptor table) for protected mode is set-up

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax	; Make all segment registers point to the data segment description address (no actual segmentation will be used)

    mov bx, STR_SWITCHED_MODE
    call pm_print

    mov eax, [KERNEL_OFFSET]
    call eax

    jmp $


%include "print.s"
%include "print_hex.s"
%include "read_disk.s"
;%include "get_memory_info.s"
%include "pm_gdt.s"
%include "pm_print.s"

KERNEL_OFFSET: dd 0x1000

STACK_BASE equ 0x99900
BOOTING_DRIVE db 0
STR_SWITCHED_MODE db "Switched to 32-bit mode.", 0

times 510-($-$$)	db 0  ; padding
dw	0xaa55  ; valid boot indicator