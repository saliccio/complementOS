[org 0x7c00]  ; bootloader memory offset
KERNEL_OFFSET equ 0x1000

mov [BOOTING_DRIVE], dl

mov ebp, 0x99900
mov esp, ebp  ; Move the stack pointer to an appropriate place

call load_kernel

lgdt [gdt_descriptor]	; Load GDT (Global Descriptor Table) into the CPU
mov eax, cr0
or eax, 1
mov cr0, eax  ; Switch to the 32-bit protected mode
jmp CODE_SEG:pm_start  ; Initiate a FAR jump to ensure the pipeline flushes (to avoid problems that may arise when executing 16-bit instructions left in the pipeline)

[bits 16]  ; Ensure 16-bit real mode to make BIOS disk interrupt to load the kernel (0x13)
load_kernel:
mov bx, KERNEL_OFFSET  ; Where to load in memory
mov dh, 40  ; Sector count
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
mov ss, ax	; Make all segment registers point to the data segment description address

mov bx, STR_SWITCHED_MODE
call pm_print

call KERNEL_OFFSET

jmp $


%include "print.asm"
%include "print_hex.asm"
%include "read_disk.asm"
%include "pm_gdt.asm"
%include "pm_print.asm"

BOOTING_DRIVE db 0

STR_REAL_MODE db "Started in 16-bit mode", 0
STR_SWITCHED_MODE db "Switched to 32-bit mode", 0

times 510-($-$$)	db 0  ; padding
dw	0xaa55  ; valid boot indicator