CC=i686-elf-gcc
AS=i686-elf-as
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -Iinclude -Icompiler/include

OBJECTS=boot.o kernel.o string.o keyboard.o ui.o userspace/userspace.o userspace/editor.o gdt.o fs/scooterfs.o memory.o scheduler.o context_switch.o compiler/src/compiler.o

all: scooterOS.iso

boot.o: boot.s
	$(AS) boot.s -o boot.o

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

userspace/%.o: userspace/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

userspace/editor.o: userspace/editor.c userspace/editor.h
	$(CC) -c userspace/editor.c -o userspace/editor.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -Iinclude

scooterOS.bin: boot.o kernel.o string.o keyboard.o ui.o userspace/userspace.o userspace/editor.o gdt.o fs/scooterfs.o memory.o scheduler.o context_switch.o
	$(CC) -T linker.ld -o scooterOS.bin -ffreestanding -O2 -nostdlib boot.o kernel.o string.o keyboard.o ui.o userspace/userspace.o userspace/editor.o gdt.o fs/scooterfs.o memory.o scheduler.o context_switch.o -lgcc

scooterOS.iso: scooterOS.bin
	mkdir -p isodir/boot/grub
	cp scooterOS.bin isodir/boot/scooterOS.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o scooterOS.iso isodir

clean:
	rm -f $(OBJECTS) scooterOS.bin scooterOS.iso
	rm -rf isodir

run: scooterOS.iso
	qemu-system-i386 -cdrom scooterOS.iso

context_switch.o: context_switch.s
	$(AS) context_switch.s -o context_switch.o
