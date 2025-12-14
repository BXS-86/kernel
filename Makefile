C_SOURCES = kernel.c
ASM_SOURCES = arch/x86_64/boot.asm

OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

CFLAGS = -ffreestanding -fno-stack-protector -fno-pic -m64 -Iinclude -O2 -nostdlib -Wall -Wextra
LDFLAGS = -nostdlib -T linker.ld -z max-page-size=0x1000 -m elf_x86_64

all: kernel.elf

kernel.elf: $(OBJ)
	ld $(LDFLAGS) $(OBJ) -o kernel.elf

iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "BXKernel" {' >> iso/boot/grub/grub.cfg
	echo '    multiboot /boot/kernel.elf' >> iso/boot/grub/grub.cfg
	echo '    boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o bxk.iso iso/

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm -f elf64 $< -o $@

clean:
	rm -f *.elf *.o arch/x86_64/*.o

run-iso: iso
	qemu-system-x86_64 -cdrom bxk.iso

.PHONY: all clean iso run-iso
