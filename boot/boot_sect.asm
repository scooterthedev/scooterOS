BITS 16
ORG 0x7C00

KERNEL_OFFSET equ 0x100000    ; Load kernel at 1MB

start:
    ; Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Save boot drive number
    mov [boot_drive], dl

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Load kernel from disk
    call load_kernel
    
    ; Switch to protected mode
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load GDT
    
    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to 32-bit code
    jmp CODE_SEG:init_pm

; Disk load function
disk_load:
    push dx
    mov ah, 0x02           ; BIOS read sector function
    mov al, dh             ; Number of sectors to read
    mov ch, 0x00           ; Cylinder number
    mov dh, 0x00           ; Head number
    mov cl, 0x02           ; Start from sector 2
    int 0x13               ; BIOS interrupt
    jc disk_error          ; Jump if error (carry flag set)
    pop dx
    cmp dh, al             ; Check if we read the correct number of sectors
    jne disk_error
    ret

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $

; Print string function
print_string:
    pusha
    mov ah, 0x0e          ; BIOS teletype function
.loop:
    lodsb                 ; Load next character
    test al, al           ; Check for null terminator
    jz .done
    int 0x10              ; Print character
    jmp .loop
.done:
    popa
    ret

; GDT
gdt_start:
    dq 0                  ; Null descriptor

gdt_code:                 ; Code segment descriptor
    dw 0xFFFF            ; Limit (bits 0-15)
    dw 0                 ; Base (bits 0-15)
    db 0                 ; Base (bits 16-23)
    db 10011010b         ; Access byte
    db 11001111b         ; Flags + Limit (bits 16-19)
    db 0                 ; Base (bits 24-31)

gdt_data:                 ; Data segment descriptor
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dd gdt_start                 ; Address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[BITS 32]
init_pm:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel
    call KERNEL_OFFSET
    jmp $

; Data
boot_drive: db 0
boot_msg: db 'Loading ScooterOS...', 13, 10, 0
disk_error_msg: db 'Disk read error!', 13, 10, 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55

load_kernel:
    mov bx, KERNEL_OFFSET  ; Set up parameters for disk_load
    mov dh, 50             ; Load 50 sectors (25KB should be enough)
    mov dl, [boot_drive]   ; Drive number saved from boot
    
    mov si, boot_msg       ; Print loading message
    call print_string
    
    call disk_load         ; Call disk_load function
    ret
