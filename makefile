KERNEL_DIR:=kernel
BOOT_DIR:=boot
OUTPUT_DIR:=out

C_SRCS := $(wildcard kernel/*.c drivers/*.c)
ASSEMBLER_SRCS := $(wildcard boot/*.asm) 
HEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJECTS := $(patsubst %.asm, %.o, $(ASSEMBLER_SRCS))
OBJECTS += $(patsubst %.c, %.o, $(C_SRCS))
# OBJECTS := $(OUTPUT_DIR)/kernel.o

CC = gcc
CFLAGS = -I include -ffreestanding -nostdlib -gdwarf-4 -m32 -ggdb3 -Wall -Wextra -fno-pic -O3  -c
# CFLAGS = -m32 -ffreestanding -Wall -Wextra -Werror -fno-pic -c

AS = nasm
ASFLAGS = -f elf32 -F dwarf -g

LD = ld
LDFLAGS = -T link.ld -m elf_i386

TARGET_NAME:=bootloader

TARGET:=$(BOOT_DIR)/$(TARGET_NAME)

DEPS:=
DEPS+= $(TARGET).asm
DEPS:= $(BOOT_DIR)/io.asm
DEPS:= $(BOOT_DIR)/io_pm.asm
DEPS:= $(BOOT_DIR)/switch_pm.asm
DEPS:= $(BOOT_DIR)/gdt.asm
DEPS:= $(BOOT_DIR)/diskio.asm
DEPS:= $(BOOT_DIR)/kernel_entry.asm
DEPS:= $(KERNEL_DIR)/kernel.c
DEPS:= $(KERNEL_DIR)/low_level.c

.PHONY: all clean run

all: $(OUTPUT_DIR)/kernel.elf $(OUTPUT_DIR)/os-image.iso

$(OUTPUT_DIR):
	mkdir -p $@/iso/boot/grub

$(OUTPUT_DIR)/os-image.iso: $(OUTPUT_DIR)/kernel.elf | $(OUTPUT_DIR)
	cp -r boot/grub/ $(OUTPUT_DIR)/iso/boot/
	cp $(OUTPUT_DIR)/kernel.elf $(OUTPUT_DIR)/iso/boot/
	grub-mkrescue -o $(OUTPUT_DIR)/os-image.iso $(OUTPUT_DIR)/iso
	echo $(OBJECTS)

$(OUTPUT_DIR)/kernel.elf: $(OBJECTS) | $(OUTPUT_DIR)
	$(LD) $(LDFLAGS) $^ -o $@

boot/%.o: boot/%.asm | $(OUTPUT_DIR)
	$(AS) $(ASFLAGS) $< -o $@

kernel/%.o: kernel/%.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $< -o $@

drivers/%.o: drivers/%.c | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf out/ $(OBJECTS)

run: all
	qemu-system-i386 -serial stdio -net none -cdrom $(OUTPUT_DIR)/os-image.iso

run_debug: all
	qemu-system-i386 -net none -cdrom $(OUTPUT_DIR)/os-image.iso -gdb tcp::26000 -S
	
