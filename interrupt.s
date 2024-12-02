.global keyboard_handler_int

keyboard_handler_int:
    pusha
    call keyboard_handler
    movb $0x20, %al
    outb %al, $0x20
    popa
    iret 