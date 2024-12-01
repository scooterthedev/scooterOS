CC=i686-elf-gcc
AS=i686-elf-as

all: scooterOS.iso

boot.o: boot.s
	$(AS) boot.s -o boot.o

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

scooterOS.bin: boot.o kernel.o linker.ld
	$(CC) -T linker.ld -o scooterOS.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

scooterOS.iso: scooterOS.bin
	mkdir -p isodir/boot/grub
	cp scooterOS.bin isodir/boot/scooterOS.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o scooterOS.iso isodir

clean:
	rm -f *.o scooterOS.bin scooterOS.iso
	rm -rf isodir

run: scooterOS.iso
	qemu-system-i386 -cdrom scooterOS.iso
