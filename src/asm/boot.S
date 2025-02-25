; Setup Grub params
MALIGN equ   1 << 0
MEMINFO equ  1 << 1
FLAGS equ    MALIGN | MEMINFO  
MAGIC equ    0x1BADB002       
CHECKSUM equ -(MAGIC + FLAGS) 

; Setup multiboot signature, flags, and checksum
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Initialize kernel stack
section .bss
align 16
stack_bottom:
    resb 16384 * 8
stack_top:

; Load initial page directory and jump to kernel entry
section .boot
global _start
_start:
    ; This is redundant for now
    jmp higher_half

; Kernel entry, setup stack pointer and enter kernel_main()
section .text
extern kernel_main
higher_half:
    mov esp, stack_top
    push ebx
    push eax
    xor ebp, ebp
    call kernel_main

; Fall-through in case kernel_main() returns (it shouldn't ever return)
halt:
    hlt
    jmp halt
