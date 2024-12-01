BITS 16
ORG 0x1000

start:
    ; Print message
    mov si, msg
    call print_string
    
    ; Halt
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

msg db 'Kernel loaded successfully! YAY!', 13, 10, 0