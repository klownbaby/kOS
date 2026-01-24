TARGET := sh

SRCS := $(wildcard src/*.c)
LIBSRCS := $(wildcard lib/*.c)

OBJS := $(patsubst src/%.c,bin/%.o,$(SRCS))
LIBOBJS := $(patsubst lib/%.c,bin/%.o,$(LIBSRCS))

EXECUTABLE := bin/$(TARGET).exe

KOSLIBDIR := ../../multiboot/boot
FSDIR := ../../fs/root

CC := i686-w64-mingw32-gcc
CFLAGS := -m32 -c -Iinclude -I../../lib -I../../include -nostdlib -mms-bitfields -fms-extensions -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone
LD := ld
LDFLAGS := -m i386pe -T linker.ld -L$(KOSLIBDIR) -lkoslib -nostdlib --image-base 0x3fff0000 --subsystem console -e __main 

.PHONY: all clean

all: clean $(LIBOBJS) $(OBJS) $(EXECUTABLE)

clean:
	rm -rf bin/*

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBOBJS): $(LIBSRCS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(LIBOBJS) $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBOBJS)
	cp $@ $(FSDIR)
