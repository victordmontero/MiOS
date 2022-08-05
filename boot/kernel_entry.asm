[bits 32]
[extern main]
[global loader]

MAGIC equ 0x1BADB002 ;multiboot magic number constant
FLAGS equ 0x0        ;multiboot flags
CHECKSUM equ -MAGIC  ;calculate the checksum
                     ;(magic number + checksum + flags) should be zero
KERNEL_STACK_SIZE equ 4096

section .bss ;uninitialize data section
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE

section .text   ;code data section
align 4         ;four byte alignment
    dd MAGIC    ;allocating values
    dd FLAGS
    dd CHECKSUM

loader:

    mov esp, kernel_stack + KERNEL_STACK_SIZE
    call main
    cli
    hlt
jmp $
