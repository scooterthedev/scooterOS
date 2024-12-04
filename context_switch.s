.global switch_task_asm

.section .text
switch_task_asm:
    # Arguments:
    # Current task pointer in %edi
    # Next task pointer in %esi

    # Save registers of current task
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    # Save ESP and EBP
    mov %esp, 0(%edi)     # Save ESP to current task
    mov %ebp, 4(%edi)     # Save EBP to current task

    # Load ESP and EBP from next task
    mov 0(%esi), %esp     # Load next task's ESP
    mov 4(%esi), %ebp     # Load next task's EBP

    # Restore segment registers
    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa

    ret
