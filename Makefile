AS = nasm
CC = gcc
LD = ld
CFLAGS = -ffreestanding -m64 -nostdlib -Wall -Wextra \
         -Iinclude \
         -Ikernel/include \
         -Idrivers/vga \
         -Idrivers/keyboard \
         -Idrivers/timer \
         -Igui/include \
         -Ifile_explorer/include \
         -Ifs/fat32 \
         -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS = -T linker.ld -nostdlib

all: scooter_os.bin user.bin

boot.bin: boot/boot_sect.asm
	$(AS) -f bin boot/boot_sect.asm -o boot.bin

kernel.elf: kernel/src/main.c kernel/src/gdt.c kernel/src/gdt_flush.s \
           kernel/src/idt.c kernel/src/io.c kernel/src/interrupts.s \
           kernel/src/idt_flush.s kernel/src/memory.c \
           drivers/vga/vga.c drivers/keyboard/keyboard.c \
           drivers/timer/timer.c gui/src/window_manager.c \
           file_explorer/src/explorer.c fs/fat32/fat32.c kernel/src/scheduler.s
	$(CC) $(CFLAGS) -c kernel/src/main.c -o kernel/main.o
	$(CC) $(CFLAGS) -c kernel/src/gdt.c -o kernel/gdt.o
	$(AS) -f elf64 kernel/src/gdt_flush.s -o kernel/gdt_flush.o
	$(CC) $(CFLAGS) -c kernel/src/idt.c -o kernel/idt.o
	$(CC) $(CFLAGS) -c kernel/src/io.c -o kernel/io.o
	$(AS) -f elf64 kernel/src/interrupts.s -o kernel/interrupts.o
	$(AS) -f elf64 kernel/src/idt_flush.s -o kernel/idt_flush.o
	$(CC) $(CFLAGS) -c kernel/src/memory.c -o kernel/memory.o
	$(CC) $(CFLAGS) -c drivers/vga/vga.c -o drivers/vga/vga.o
	$(CC) $(CFLAGS) -c drivers/keyboard/keyboard.c -o drivers/keyboard/keyboard.o
	$(CC) $(CFLAGS) -c drivers/timer/timer.c -o drivers/timer/timer.o
	$(CC) $(CFLAGS) -c gui/src/window_manager.c -o gui/src/window_manager.o
	$(CC) $(CFLAGS) -c file_explorer/src/explorer.c -o file_explorer/src/explorer.o
	$(CC) $(CFLAGS) -c fs/fat32/fat32.c -o fs/fat32/fat32.o
	$(AS) -f elf64 kernel/src/scheduler.s -o kernel/scheduler.o
	$(LD) $(LDFLAGS) --oformat=elf64-x86-64 \
		kernel/main.o kernel/gdt.o kernel/gdt_flush.o \
		kernel/idt.o kernel/io.o kernel/interrupts.o kernel/idt_flush.o \
		kernel/memory.o \
		drivers/vga/vga.o drivers/keyboard/keyboard.o drivers/timer/timer.o \
		gui/src/window_manager.o file_explorer/src/explorer.o fs/fat32/fat32.o \
		kernel/scheduler.o -o kernel.elf

scooter_os.bin: boot.bin kernel.elf
	objcopy -O binary kernel.elf kernel.bin
	ls -l kernel.bin
	dd if=/dev/zero of=scooter_os.bin bs=512 count=2880
	dd if=boot.bin of=scooter_os.bin conv=notrunc
	dd if=kernel.bin of=scooter_os.bin conv=notrunc seek=1

user.elf: user_space/src/user_main.c user_space/src/user_task.c user_space/src/switch_to_user.s
	$(CC) $(CFLAGS) -c user_space/src/user_main.c -o user_space/src/user_main.o
	$(CC) $(CFLAGS) -c user_space/src/user_task.c -o user_space/src/user_task.o
	$(AS) -f elf64 user_space/src/switch_to_user.s -o user_space/switch_to_user.o
	$(LD) -T user_linker.ld -o user_space/user_main.elf user_space/src/user_main.o user_space/src/user_task.o user_space/switch_to_user.o

user.bin: user.elf
	objcopy -O binary user.elf user.bin

clean:
	rm -f boot.bin kernel.elf kernel/*.o drivers/vga/*.o drivers/keyboard/*.o drivers/timer/*.o \
	      gui/src/*.o file_explorer/src/*.o fs/fat32/*.o \
	      kernel/scheduler.o \
	      scooter_os.bin \
	      user_space/src/*.o user_space/switch_to_user.o user_space/user_main.elf user.bin

run: all
	qemu-system-x86_64 -drive format=raw,file=scooter_os.bin -d int -D qemu.log -monitor stdio