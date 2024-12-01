BITS 16
ORG 0x7c00

; Initialize segment registers
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; Read kernel from disk
mov ah, 0x02        ; BIOS read sector function
mov al, 2           ; Number of sectors to read
mov ch, 0           ; Cylinder number
mov cl, 2           ; Sector number (1 is boot sector)
mov dh, 0           ; Head number
mov dl, [boot_drive]; Drive number
mov bx, 0x1000      ; Load kernel to ES:BX = 0x0000:0x1000
int 0x13
jc disk_error

; Jump to kernel
jmp 0x0:0x1000

disk_error:
    mov si, error_msg
    call print_string
    jmp $

print_string:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

error_msg db 'Error loading kernel!', 0
boot_drive db 0

times 510-($-$$) db 0
dw 0xaa55 