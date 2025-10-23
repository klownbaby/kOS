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

; Kernel entry, setup stack pointer and enter KernelMain()
section .boot
global _start
extern KernelMain
_start:
    ; Set stack pointer
    mov esp, stack_top
    push ebx
    push eax
    xor ebp, ebp

    ; Finally, jump to high level language
    call KernelMain

; Fall-through in case KernelMain() returns (it shouldn't ever return)
halt:
    hlt
    jmp halt
