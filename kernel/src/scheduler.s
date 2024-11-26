section .text
global switch_to_user

switch_to_user:
    ; Save current kernel context
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Load user-space stack
    pop r15
    ; Assume stack pointer is passed via stack in tasks[current_task].stack
    mov rsp, r15

    ; Jump to user task
    jmp rax 
