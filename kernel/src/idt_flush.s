section .text
global idt_flush

section .note.GNU-stack noalloc noexec nowrite progbits

idt_flush:
    lidt [rdi]      ; Load IDT directly from the address in rdi
    ret