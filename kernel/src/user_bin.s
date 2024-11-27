section .rodata
global _binary_user_bin_start
global _binary_user_bin_end

_binary_user_bin_start:
    incbin "user_space/user.elf.bin"
_binary_user_bin_end:
