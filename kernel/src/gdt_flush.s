section .text
global gdt_flush

gdt_flush:
    lgdt [rdi]            ; Load GDT from the address in RDI
    mov ax, 0x10         ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; Far jump to reload CS and flush the instruction pipeline
    push 0x08            ; Code segment selector
    push .reload_cs      ; Return address
    retfq               ; Far return to reload CS
.reload_cs:
    ret