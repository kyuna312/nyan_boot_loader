# Tools
ASM = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

# Files
BUILD_DIR = build
BOOT_SRC = boot.asm
KERNEL_SRC = kernel.c
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/nyanboot.img

# Flags for 16-bit compatibility
ASM_FLAGS = -f bin
CFLAGS = -m32 -march=i386 -mpreferred-stack-boundary=2 \
         -fno-pie -fno-stack-protector -nostdlib \
         -fno-builtin -Os -c -fno-asynchronous-unwind-tables \
         -mgeneral-regs-only
LDFLAGS = -m elf_i386 --oformat binary -Ttext 0x1000 \
          -nostdlib -static

.PHONY: all clean run

all: $(IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOT_BIN): $(BOOT_SRC) | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

$(KERNEL_BIN): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $(BUILD_DIR)/kernel.o
	$(LD) $(LDFLAGS) $(BUILD_DIR)/kernel.o -o $@

$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOT_BIN) of=$@ conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ seek=1 conv=notrunc bs=512

run: $(IMAGE)
	qemu-system-i386 -drive file=$(IMAGE),format=raw,if=floppy -boot a

clean:
	rm -rf $(BUILD_DIR)
