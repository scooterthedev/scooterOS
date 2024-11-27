ENTRY(user_main)
OUTPUT_FORMAT(elf64-x86-64)
SECTIONS
{
    . = 2M; /* User-space starts at 2MB */

    .user_text BLOCK(4K) : ALIGN(4K) {
        _binary_user_bin_start = .;
        *(.user_text)
        _binary_user_bin_end = .;
    }

    .user_task_text BLOCK(4K) : ALIGN(4K) {
        _binary_user_task_bin_start = .;
        *(.user_task_text)
        _binary_user_task_bin_end = .;
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
}
