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
    mov ecx, (initial_pgdir - 0xC0000000)
    mov cr3, ecx

    mov ecx, cr4
    or ecx, 0x10
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

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

; Initialize page directory
section .data
align 4096
global initial_pgdir
initial_pgdir:
    ; Map PDE 0x0 to 0x10000000 and set flags
    dd 10000011b
    ; Map PDE 0xc00
    times 768-1 dd 0

    ; Set page flags
    dd (0 << 22) | 10000011b
    dd (1 << 22) | 10000011b
    dd (2 << 22) | 10000011b
    dd (3 << 22) | 10000011b
    times 256-4 dd 0
