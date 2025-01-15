[bits 32]  ; using 32-bit protected mode

global pm_print

VIDEO_MEM_ADDRESS equ 0xb8000
COLOR_WHITE equ 0x0f

pm_print:
	pusha
	mov edx, VIDEO_MEM_ADDRESS
pm_print_loop:
	mov al, [ebx]
	mov ah, COLOR_WHITE
	cmp al, 0
	je pm_print_end
	mov [edx], ax
	add ebx, 1
	add edx, 2
	jmp pm_print_loop
pm_print_end:
	popa
	ret