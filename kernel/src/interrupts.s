[extern isr_general_handler]  ; isr.c
[extern irq_general_handler]  ; isr.c

; Housekeeping routine for interrupt service routines
isr_housekeeping:
    ; 1. Save current CPU state
	pusha  ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
	mov ax, ds  ; Lower 16-bits of eax = ds.
	push eax  ; Save current data segment entry offset
	
    ; Make all segment registers to point at kernel data segment. This is done to switch to the kernel mode.
    mov ax, 0x10  ; Kernel data segment entry offset (boot/pm_gdt.asm)
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
    ; 2. Call C general handler
	call isr_general_handler
	
    ; 3. Restore state
	pop eax 
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8 ; Cleans up the pushed error code and pushed ISR number
	sti
	iret ; Pops cs, eip, eflags, ss, esp

irq_housekeeping:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_general_handler

    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret
	
global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31

global _irq0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15

; NOTE: These routines are not the real handlers, they just perform low-level tasks. Real handlers are defined in interrupts.c

; 0: Divide By Zero Exception
_isr0:
    cli  ; The very first thing to do when an ISR runs is to clear interrupt flag (disable maskable interrupts)
    push byte 0  ; Push error code (some exceptions already do that)
    push byte 0  ; Push interrupt number
    jmp isr_housekeeping  ; Jump to the housekeeping code

; 1: Debug Exception
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_housekeeping

; 2: Non Maskable Interrupt Exception
_isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_housekeeping

; 3: Int 3 Exception
_isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_housekeeping

; 4: INTO Exception
_isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_housekeeping

; 5: Out of Bounds Exception
_isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_housekeeping

; 6: Invalid Opcode Exception
_isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_housekeeping

; 7: Coprocessor Not Available Exception
_isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_housekeeping

; 8: Double Fault Exception (With Error Code)
_isr8:
    cli
    push byte 8
    jmp isr_housekeeping

; 9: Coprocessor Segment Overrun Exception
_isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_housekeeping

; 10: Bad TSS Exception (With Error Code)
_isr10:
    cli
    push byte 10
    jmp isr_housekeeping

; 11: Segment Not Present Exception (With Error Code)
_isr11:
    cli
    push byte 11
    jmp isr_housekeeping

; 12: Stack Fault Exception (With Error Code)
_isr12:
    cli
    push byte 12
    jmp isr_housekeeping

; 13: General Protection Fault Exception (With Error Code)
_isr13:
    cli
    push byte 13
    jmp isr_housekeeping

; 14: Page Fault Exception (With Error Code)
_isr14:
    cli
    push byte 14
    jmp isr_housekeeping

; 15: Reserved Exception
_isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_housekeeping

; 16: Floating Point Exception
_isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_housekeeping

; 17: Alignment Check Exception
_isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_housekeeping

; 18: Machine Check Exception
_isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_housekeeping

; 19: Reserved
_isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_housekeeping

; 20: Reserved
_isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_housekeeping

; 21: Reserved
_isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_housekeeping

; 22: Reserved
_isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_housekeeping

; 23: Reserved
_isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_housekeeping

; 24: Reserved
_isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_housekeeping

; 25: Reserved
_isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_housekeeping

; 26: Reserved
_isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_housekeeping

; 27: Reserved
_isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_housekeeping

; 28: Reserved
_isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_housekeeping

; 29: Reserved
_isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_housekeeping

; 30: Reserved
_isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_housekeeping

; 31: Reserved
_isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_housekeeping

_irq0:
	cli
	push byte 0
	push byte 32
	jmp irq_housekeeping

_irq1:
	cli
	push byte 1
	push byte 33
	jmp irq_housekeeping

_irq2:
	cli
	push byte 2
	push byte 34
	jmp irq_housekeeping

_irq3:
	cli
	push byte 3
	push byte 35
	jmp irq_housekeeping

_irq4:
	cli
	push byte 4
	push byte 36
	jmp irq_housekeeping

_irq5:
	cli
	push byte 5
	push byte 37
	jmp irq_housekeeping

_irq6:
	cli
	push byte 6
	push byte 38
	jmp irq_housekeeping

_irq7:
	cli
	push byte 7
	push byte 39
	jmp irq_housekeeping

_irq8:
	cli
	push byte 8
	push byte 40
	jmp irq_housekeeping

_irq9:
	cli
	push byte 9
	push byte 41
	jmp irq_housekeeping

_irq10:
	cli
	push byte 10
	push byte 42
	jmp irq_housekeeping

_irq11:
	cli
	push byte 11
	push byte 43
	jmp irq_housekeeping

_irq12:
	cli
	push byte 12
	push byte 44
	jmp irq_housekeeping

_irq13:
	cli
	push byte 13
	push byte 45
	jmp irq_housekeeping

_irq14:
	cli
	push byte 14
	push byte 46
	jmp irq_housekeeping

_irq15:
	cli
	push byte 15
	push byte 47
	jmp irq_housekeeping