; dl = drive number. The caller sets it as a parameter and gets it from BIOS (0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2)
disk_load:
    pusha
    push dx
    mov ah, 0x02
    mov al, dh
    mov cl, 0x02
    mov ch, 0x00
    mov dh, 0x00
    int 0x13      ; BIOS interrupt
    jc .disk_error
    pop dx
    cmp al, dh
    jne .sectors_error
    popa
    ret
.disk_error:
    mov bx, DISK_ERROR
    call print
    call print_nl
    mov dh, ah
    call print_hex
    jmp .loop
.sectors_error:
    mov bx, SECTORS_ERROR
    call print
.loop:
    jmp $

DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0