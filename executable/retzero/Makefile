TARGET := retzero
SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,bin/%.o,$(SRCS))
EXECUTABLE := bin/$(TARGET).exe

LIBDIR := ../../multiboot/boot
FSDIR := ../../fs/root

CC := i686-w64-mingw32-gcc
CFLAGS := -m32 -c -I include -I ../../lib -I ../../include -nostdlib -mms-bitfields -fms-extensions -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone
LD := ld
LDFLAGS := -m i386pe -L$(LIBDIR) -lkoslib -nostdlib --image-base 0x3fff0000 --subsystem console 

.PHONY: all clean

all: clean $(EXECUTABLE)

clean:
	rm -rf bin/*

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $<

$(EXECUTABLE): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $<
	cp $@ $(FSDIR)
