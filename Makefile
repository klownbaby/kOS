# 
# 
# Shield your eyes... this whole Makefile is awful.
# 
# 

BOOTDIR := ./multiboot/boot
OBJECTDIR := ./bin
SRCDIR := ./src
ASMDIR := $(SRCDIR)/asm
DRIVERSDIR := $(SRCDIR)/drivers
INCLUDEDIR := ./include

EXECUTABLESDIR := ./executable
EXECUTABLESRCS := $(wildcard $(EXECUTABLESDIR)/src/*.c)
EXECUTABLEOBJS := $(patsubst $(EXECUTABLESDIR)/src/%.c,$(EXECUTABLESDIR)/bin/%.o,$(EXECUTABLESRCS))
EXECUTABLES := $(patsubst $(EXECUTABLESDIR)/src/%.c,$(EXECUTABLESDIR)/bin/%.exe,$(EXECUTABLESRCS))

GNUEFIDIR := ./gnu-efi
EFISRCS := $(wildcard $(SRCDIR)/boot/*.c)
EFIOBJS := $(patsubst $(SRCDIR)/boot/%.c,$(OBJECTDIR)/%.o,$(EFISRCS)) compat_mode.o
EFILIBS := $(patsubst $(SRCDIR)/boot/%.c,$(OBJECTDIR)/%.so,$(EFISRCS))
EFIIMGS := $(patsubst $(SRCDIR)/boot/%.c,$(OBJECTDIR)/%.efi,$(EFISRCS))

RUST ?= 0
RUSTTARGET := x86_64-kos
RUSTBIN := ./lib/target
RUSTENTRY := $(RUSTBIN)/$(RUSTTARGET)/debug/libkOS.a

CC := i686-elf
EXECC := i686-w64-mingw32-gcc -m32
DOCKER := docker run -it --rm -v .:/root/env kos
ASC := nasm -f elf32
EMU := qemu-system-x86_64

KERNELTARGET := kos
OBJECTS := $(OBJECTDIR)/*.o
CTARGETS := $(SRCDIR)/*.c $(DRIVERSDIR)/*.c
ASMTARGETS := $(ASMDIR)/*.S
ISO := $(OBJECTDIR)/$(KERNELTARGET).iso

.PHONY: all kernel env image verify grub grub-rescue fs vfs run debug clean exec exec-clean efi-build efi-run

all: kernel image run

grub: kernel image run

efi: kernel image efi-build efi-run

# Build kernel
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
	$(DOCKER) ld -m i386pe -shared -e KernelMain --image-base 0x100000 --defsym=_dynmod_start=1 --defsym=_dynmod_end=1 --defsym=_kernel_start=1 --defsym=_kernel_end=1 --no-leading-underscore -o $(BOOTDIR)/$(KERNELTARGET).exe --subsystem console -ffreestanding -O2 -nostdlib $(OBJECTS)
endif

# Build executables
exec: exec-clean $(EXECUTABLES)

$(EXECUTABLEOBJS): $(EXECUTABLESRCS)
	$(DOCKER) $(EXECC) -c -nostdlib -mms-bitfields -fms-extensions -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -o $@ $<

$(EXECUTABLES): $(EXECUTABLEOBJS)
	$(DOCKER) ld -m i386pe -Lmultiboot/boot -lkoslib -nostdlib --image-base 0x3fff0000 --subsystem console -o $@ $<
	cp $@ ./fs/root/

# Build bootable EFI image
efi-build: clean $(EFIIMGS)

$(EFIOBJS): $(EFISRCS)
	$(ASC) $(ASMDIR)/compat.S -o $(OBJECTDIR)/compat.o
	$(DOCKER) gcc -I$(GNUEFIDIR)/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c $< -o $@

$(EFILIBS): $(EFIOBJS)
	$(DOCKER) ld -shared -Bsymbolic -L$(GNUEFIDIR)/x86_64/lib -L$(GNUEFIDIR)/x86_64/gnuefi -T$(GNUEFIDIR)/gnuefi/elf_x86_64_efi.lds $(GNUEFIDIR)/x86_64/gnuefi/crt0-efi-x86_64.o $< -o $@ -lgnuefi -lefi

$(EFIIMGS): $(EFILIBS)
	$(DOCKER) objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $< $@

efi-run:
	qemu-system-x86_64 -bios OVMF.fd -hda $(ISO) -hdb ./fs/fs.img

# Misc environment/GRUB shit
env:
	docker build env -t kos

image:
	$(DOCKER) make grub-rescue

fs:
	$(DOCKER) make vfs

vfs:
	dd if=/dev/zero of=fs/fs.img bs=1M count=10
	mkfs.fat -F 16 -n 0 fs/fs.img
	mcopy -svi fs/fs.img fs/root/* ::/

verify:
	grub-file --is-x86-multiboot $(BOOTDIR)/$(KERNELTARGET).bin

grub-rescue:
	grub-mkrescue -o $(OBJECTDIR)/$(KERNELTARGET).iso multiboot

run: $(ISO)
	$(EMU) -hda $(ISO) -hdb ./fs/fs.img

debug: clean kernel image $(ISO)
	$(EMU) -hda $(ISO) -s -S -hdb ./fs/fs.img

exec-clean:
	rm -rf $(EXECUTABLESDIR)/bin/*.exe
	rm -rf $(EXECUTABLESDIR)/bin/*.o

clean:
	rm -rf $(OBJECTDIR)/*.*
	rm -rf $(KERNELTARGET).iso
	rm -rf $(BOOTDIR)/$(KERNELTARGET).bin
	rm -rf $(RUSTBIN)
	mkdir -p $(OBJECTDIR)
