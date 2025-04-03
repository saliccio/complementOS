[bits 16]

; ah = Always 2 to issue a read disk interrupt.
; al = Total sector count to read.
; ch = Cylinder number.
; cl = Sector number.
; dh = Head number.
; dl = Drive number. The caller sets it as a parameter and gets it from BIOS (0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2).
; es:bx = Buffer address.
disk_load:
    cld                                        ; Clear direction flag

    mov dl, 0x80                               ; Hard drive selection

load_ap_code:
    mov cx, 0x2                                ; Start from sector 2 (Skip sector 1)
    mov dh, 0x00                               ; Start at Head 0

    mov ax, AP_CODE_SEGMENT                    ; Give AP code segment
    mov es, ax                                 ; Move it to the segment register
    mov bx, 0                                  ; Set offset to 0  

    mov ah, 0x02                               ; BIOS Read Sector function
    mov al, 0x01                               ; Read one sector
    int 0x13                                   ; Call BIOS interrupt
    jc read_error                              ; Jump if error occurs

    mov cx, 0x3                                ; Increment sector
    mov dx, 0x80                               ; Ensure clean value after BIOS interrupt

    mov ax, KERNEL_PHYSICAL_ADDRESS_SEGMENT    ; Give kernel code segment
    mov es, ax                                 ; Move it to the segment register

next_sector:
    push cx                                    ; Save CX (cylinder & sector)
    push dx                                    ; Save DX (head & drive)

    mov ah, 0x02                               ; BIOS Read Sector function
    mov al, 0x01                               ; Read one sector
    int 0x13                                   ; Call BIOS interrupt
    jc read_error                              ; Jump if error occurs

    add bx, 512                                ; Move buffer forward by 512 bytes
    cmp bx, 0                                  ; Check if 64 KB segment limit is overrun
    jg es_is_valid                             ; If not, continue

    push ax
    mov ax, es
    add ax, 4096
    mov es, ax                                 ; Increase segment register to the next 64 KB segment
    pop ax

es_is_valid:
    pop dx                                     ; Restore DX (head & drive)
    pop cx                                     ; Restore CX (cylinder & sector)

    inc cl                                     ; Move to next sector

    cmp cl, 64                                 ; If sector > 63, reset and increment head
    jl next_sector                             ; If not, continue

    mov cl, 0x01                               ; Reset sector number to 1
    inc dh                                     ; Move to next head

    cmp dh, 255                                ; If head > 255, reset and increment cylinder
    jl next_sector                             ; If not, continue

    mov dh, 0x00                               ; Reset head number to 0
    inc ch                                     ; Move to next cylinder

    cmp ch, 1024                               ; If cylinder > 1023, done
    jl next_sector                             ; If not, continue

    ret

read_error:
    mov bx, DISK_ERROR
    call print
    jmp $                                      ; Stop execution

DISK_ERROR: db "Disk error", 0