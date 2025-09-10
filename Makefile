BOOTDIR := ./multiboot/boot
OBJECTDIR := ./bin
SRCDIR := ./src
ASMDIR := $(SRCDIR)/asm
DRIVERSDIR := $(SRCDIR)/drivers
INCLUDEDIR := ./include

RUST ?= 0
RUSTTARGET := x86_64-kos
RUSTBIN := ./lib/target
RUSTENTRY := $(RUSTBIN)/$(RUSTTARGET)/debug/libkOS.a

CC := i686-elf
DOCKER := docker run -it --rm -v .:/root/env kos
ASC := nasm -f elf32
EMU := qemu-system-x86_64 -hda

KERNELTARGET := kos
OBJECTS := $(OBJECTDIR)/*.o
CTARGETS := $(SRCDIR)/*.c $(DRIVERSDIR)/*.c
ASMTARGETS := $(ASMDIR)/*.S
ISO := $(OBJECTDIR)/$(KERNELTARGET).iso


.PHONY: all kernel env image verify grub fs vfs run debug clean

all: kernel image run

kernel: clean
	$(ASC) $(ASMDIR)/boot.S -o $(OBJECTDIR)/boot.o
	$(ASC) $(ASMDIR)/gdt.S -o $(OBJECTDIR)/_gdt.o
	$(ASC) $(ASMDIR)/idt.S -o $(OBJECTDIR)/_idt.o
	$(DOCKER) $(CC)-gcc -g -I $(INCLUDEDIR) -c $(CTARGETS) -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wno-incompatible-pointer-types

	mv ./*.o $(OBJECTDIR)

ifeq ($(RUST),1)
	CARGO_TARGET_DIR=$(RUSTBIN) cargo build --target ./lib/$(RUSTTARGET).json
	mv $(RUSTBIN)/$(RUSTTARGET)/debug/libkr.a $(RUSTENTRY)
	$(DOCKER) $(CC)-gcc -T linker.ld -o $(BOOTDIR)/$(KERNELTARGET).bin -ffreestanding -O2 -nostdlib $(OBJECTS) $(RUSTENTRY) -lgcc
else
	$(DOCKER) $(CC)-gcc -T linker.ld -o $(BOOTDIR)/$(KERNELTARGET).bin -ffreestanding -O2 -nostdlib $(OBJECTS) -lgcc
endif

obj:
	objcopy -j .text -j .sdata -j .data -j .rodata -j .rdata -j .bss -j .reloc --target=pei-x86-64 your_elf_file.so boot.efi

env:
	docker build env -t kos

image:
	$(DOCKER) make grub

fs:
	$(DOCKER) make vfs

vfs:
	dd if=/dev/zero of=fs/fs.img bs=1M count=10
	mkfs.fat -F 16 -n 0 fs/fs.img
	mcopy -svi fs/fs.img fs/root/* ::/

verify:
	grub-file --is-x86-multiboot $(BOOTDIR)/$(KERNELTARGET).bin

grub:
	grub-mkrescue -o $(OBJECTDIR)/$(KERNELTARGET).iso multiboot

run: $(ISO)
	$(EMU) $(ISO) -hdb ./fs/fs.img

efi:
	$(EMU) $(ISO) -hdb ./fs/fs.img -drive if=pflash,format=raw,readonly=on,file=./OVMF.fd

debug: clean kernel image $(ISO)
	$(EMU) $(ISO) -s -S -hdb fs.img

clean:
	rm -rf $(OBJECTDIR)/*.*
	rm -rf $(KERNELTARGET).iso
	rm -rf $(BOOTDIR)/$(KERNELTARGET).bin
	rm -rf $(RUSTBIN)
	mkdir -p $(OBJECTDIR)
