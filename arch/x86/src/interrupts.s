[bits 64]
[extern exc_general_handler]  ; isr.c
[extern irq_general_handler]  ; isr.c

; Housekeeping routine for exception routines
exc_housekeeping:
    ; 1. Save current CPU state
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
	
    ; 2. Call C general handler
	; Pass the pointer to the stack (which now contains our register structure)
    mov rdi, rsp  ; First argument in x64 calling convention is in rdi
    call exc_general_handler

    ; 3. Restore state
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
	
    add esp, 16 ; Cleans up the pushed error code and pushed exception number
    sti
	iretq ; Pops cs, eip, eflags, ss, esp

irq_housekeeping:
    ; 1. Save current CPU state
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call irq_general_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add esp, 16
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
    push 0  ; Push error code (some exceptions already do that)
    push 0  ; Push interrupt number
    jmp exc_housekeeping  ; Jump to the housekeeping code

; 1: Debug Exception
_exc1:
    cli
    push 0
    push 1
    jmp exc_housekeeping

; 2: Non Maskable Interrupt Exception
_exc2:
    cli
    push 0
    push 2
    jmp exc_housekeeping

; 3: Int 3 Exception
_exc3:
    cli
    push 0
    push 3
    jmp exc_housekeeping

; 4: INTO Exception
_exc4:
    cli
    push 0
    push 4
    jmp exc_housekeeping

; 5: Out of Bounds Exception
_exc5:
    cli
    push 0
    push 5
    jmp exc_housekeeping

; 6: Invalid Opcode Exception
_exc6:
    cli
    push 0
    push 6
    jmp exc_housekeeping

; 7: Coprocessor Not Available Exception
_exc7:
    cli
    push 0
    push 7
    jmp exc_housekeeping

; 8: Double Fault Exception (With Error Code)
_exc8:
    cli
    push 8
    jmp exc_housekeeping

; 9: Coprocessor Segment Overrun Exception
_exc9:
    cli
    push 0
    push 9
    jmp exc_housekeeping

; 10: Bad TSS Exception (With Error Code)
_exc10:
    cli
    push 10
    jmp exc_housekeeping

; 11: Segment Not Present Exception (With Error Code)
_exc11:
    cli
    push 11
    jmp exc_housekeeping

; 12: Stack Fault Exception (With Error Code)
_exc12:
    cli
    push 12
    jmp exc_housekeeping

; 13: General Protection Fault Exception (With Error Code)
_exc13:
    cli
    push 13
    jmp exc_housekeeping

; 14: Page Fault Exception (With Error Code)
_exc14:
    cli
    push 14
    jmp exc_housekeeping

; 15: Reserved Exception
_exc15:
    cli
    push 0
    push 15
    jmp exc_housekeeping

; 16: Floating Point Exception
_exc16:
    cli
    push 0
    push 16
    jmp exc_housekeeping

; 17: Alignment Check Exception
_exc17:
    cli
    push 0
    push 17
    jmp exc_housekeeping

; 18: Machine Check Exception
_exc18:
    cli
    push 0
    push 18
    jmp exc_housekeeping

; 19: Reserved
_exc19:
    cli
    push 0
    push 19
    jmp exc_housekeeping

; 20: Reserved
_exc20:
    cli
    push 0
    push 20
    jmp exc_housekeeping

; 21: Reserved
_exc21:
    cli
    push 0
    push 21
    jmp exc_housekeeping

; 22: Reserved
_exc22:
    cli
    push 0
    push 22
    jmp exc_housekeeping

; 23: Reserved
_exc23:
    cli
    push 0
    push 23
    jmp exc_housekeeping

; 24: Reserved
_exc24:
    cli
    push 0
    push 24
    jmp exc_housekeeping

; 25: Reserved
_exc25:
    cli
    push 0
    push 25
    jmp exc_housekeeping

; 26: Reserved
_exc26:
    cli
    push 0
    push 26
    jmp exc_housekeeping

; 27: Reserved
_exc27:
    cli
    push 0
    push 27
    jmp exc_housekeeping

; 28: Reserved
_exc28:
    cli
    push 0
    push 28
    jmp exc_housekeeping

; 29: Reserved
_exc29:
    cli
    push 0
    push 29
    jmp exc_housekeeping

; 30: Reserved
_exc30:
    cli
    push 0
    push 30
    jmp exc_housekeeping

; 31: Reserved
_exc31:
    cli
    push 0
    push 31
    jmp exc_housekeeping

_irq0:
	cli
	push 0
	push 32
	jmp irq_housekeeping

_irq1:
	cli
	push 1
	push 33
	jmp irq_housekeeping

_irq2:
	cli
	push 2
	push 34
	jmp irq_housekeeping

_irq3:
	cli
	push 3
	push 35
	jmp irq_housekeeping

_irq4:
	cli
	push 4
	push 36
	jmp irq_housekeeping

_irq5:
	cli
	push 5
	push 37
	jmp irq_housekeeping

_irq6:
	cli
	push 6
	push 38
	jmp irq_housekeeping

_irq7:
	cli
	push 7
	push 39
	jmp irq_housekeeping

_irq8:
	cli
	push 8
	push 40
	jmp irq_housekeeping

_irq9:
	cli
	push 9
	push 41
	jmp irq_housekeeping

_irq10:
	cli
	push 10
	push 42
	jmp irq_housekeeping

_irq11:
	cli
	push 11
	push 43
	jmp irq_housekeeping

_irq12:
	cli
	push 12
	push 44
	jmp irq_housekeeping

_irq13:
	cli
	push 13
	push 45
	jmp irq_housekeeping

_irq14:
	cli
	push 14
	push 46
	jmp irq_housekeeping

_irq15:
	cli
	push 15
	push 47
	jmp irq_housekeeping