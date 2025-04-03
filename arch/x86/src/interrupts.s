[bits 64]
[extern exc_general_handler]  ; isr.c
[extern irq_general_handler]  ; isr.c

; Housekeeping routine for exception routines
exc_housekeeping:
    ; 1. Save current CPU state
	push rdi
	push rsi
	push rbp
	push rsp
	push rbx
	push rdx
	push rcx
	push rax
	
    ; 2. Call C general handler
	call exc_general_handler

    ; 3. Restore state
	pop rax
	pop rcx
	pop rdx
	pop rbx
	pop rsp
	pop rbp
	pop rsi
	pop rdi
	
    add esp, 16 ; Cleans up the pushed error code and pushed exception number
    sti
	iretq ; Pops cs, eip, eflags, ss, esp

irq_housekeeping:
	push rdi
	push rsi
	push rbp
	push rsp
	push rbx
	push rdx
	push rcx
	push rax

    mov ax, ds

    call irq_general_handler

	pop rax
	pop rcx
	pop rdx
	pop rbx
	pop rsp
	pop rbp
	pop rsi
	pop rdi

    add esp, 8
    sti
    iretq
	
global _exc0
global _exc1
global _exc2
global _exc3
global _exc4
global _exc5
global _exc6
global _exc7
global _exc8
global _exc9
global _exc10
global _exc11
global _exc12
global _exc13
global _exc14
global _exc15
global _exc16
global _exc17
global _exc18
global _exc19
global _exc20
global _exc21
global _exc22
global _exc23
global _exc24
global _exc25
global _exc26
global _exc27
global _exc28
global _exc29
global _exc30
global _exc31

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
_exc0:
    cli  ; The very first thing to do when an ISR runs is to clear interrupt flag (disable maskable interrupts)
    push byte 0  ; Push error code (some exceptions already do that)
    push byte 0  ; Push interrupt number
    jmp exc_housekeeping  ; Jump to the housekeeping code

; 1: Debug Exception
_exc1:
    cli
    push byte 0
    push byte 1
    jmp exc_housekeeping

; 2: Non Maskable Interrupt Exception
_exc2:
    cli
    push byte 0
    push byte 2
    jmp exc_housekeeping

; 3: Int 3 Exception
_exc3:
    cli
    push byte 0
    push byte 3
    jmp exc_housekeeping

; 4: INTO Exception
_exc4:
    cli
    push byte 0
    push byte 4
    jmp exc_housekeeping

; 5: Out of Bounds Exception
_exc5:
    cli
    push byte 0
    push byte 5
    jmp exc_housekeeping

; 6: Invalid Opcode Exception
_exc6:
    cli
    push byte 0
    push byte 6
    jmp exc_housekeeping

; 7: Coprocessor Not Available Exception
_exc7:
    cli
    push byte 0
    push byte 7
    jmp exc_housekeeping

; 8: Double Fault Exception (With Error Code)
_exc8:
    cli
    push byte 8
    jmp exc_housekeeping

; 9: Coprocessor Segment Overrun Exception
_exc9:
    cli
    push byte 0
    push byte 9
    jmp exc_housekeeping

; 10: Bad TSS Exception (With Error Code)
_exc10:
    cli
    push byte 10
    jmp exc_housekeeping

; 11: Segment Not Present Exception (With Error Code)
_exc11:
    cli
    push byte 11
    jmp exc_housekeeping

; 12: Stack Fault Exception (With Error Code)
_exc12:
    cli
    push byte 12
    jmp exc_housekeeping

; 13: General Protection Fault Exception (With Error Code)
_exc13:
    cli
    push byte 13
    jmp exc_housekeeping

; 14: Page Fault Exception (With Error Code)
_exc14:
    cli
    push byte 14
    jmp exc_housekeeping

; 15: Reserved Exception
_exc15:
    cli
    push byte 0
    push byte 15
    jmp exc_housekeeping

; 16: Floating Point Exception
_exc16:
    cli
    push byte 0
    push byte 16
    jmp exc_housekeeping

; 17: Alignment Check Exception
_exc17:
    cli
    push byte 0
    push byte 17
    jmp exc_housekeeping

; 18: Machine Check Exception
_exc18:
    cli
    push byte 0
    push byte 18
    jmp exc_housekeeping

; 19: Reserved
_exc19:
    cli
    push byte 0
    push byte 19
    jmp exc_housekeeping

; 20: Reserved
_exc20:
    cli
    push byte 0
    push byte 20
    jmp exc_housekeeping

; 21: Reserved
_exc21:
    cli
    push byte 0
    push byte 21
    jmp exc_housekeeping

; 22: Reserved
_exc22:
    cli
    push byte 0
    push byte 22
    jmp exc_housekeeping

; 23: Reserved
_exc23:
    cli
    push byte 0
    push byte 23
    jmp exc_housekeeping

; 24: Reserved
_exc24:
    cli
    push byte 0
    push byte 24
    jmp exc_housekeeping

; 25: Reserved
_exc25:
    cli
    push byte 0
    push byte 25
    jmp exc_housekeeping

; 26: Reserved
_exc26:
    cli
    push byte 0
    push byte 26
    jmp exc_housekeeping

; 27: Reserved
_exc27:
    cli
    push byte 0
    push byte 27
    jmp exc_housekeeping

; 28: Reserved
_exc28:
    cli
    push byte 0
    push byte 28
    jmp exc_housekeeping

; 29: Reserved
_exc29:
    cli
    push byte 0
    push byte 29
    jmp exc_housekeeping

; 30: Reserved
_exc30:
    cli
    push byte 0
    push byte 30
    jmp exc_housekeeping

; 31: Reserved
_exc31:
    cli
    push byte 0
    push byte 31
    jmp exc_housekeeping

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