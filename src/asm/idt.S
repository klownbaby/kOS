global LoadIdt

[extern IsrHandler]
[extern IrqHandler]

%macro ISR_NO_ERR 1
    global isr%1

    isr%1:
        cli
        push long 0
        push long %1
        jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
    global isr%1

    isr%1:
        cli
        push long %1
        jmp isr_common_stub
%endmacro

%macro IRQ 1
    global irq%1

    irq%1:
        cli
        push long %1
        push long (32 + %1)
        jmp irq_common_stub
%endmacro

LoadIdt:
    mov eax, [esp + 4]
    lidt [eax]
    sti
    ret

; Common ISR stub
isr_common_stub:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
	
    call IsrHandler
	
    pop eax 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    sti
    iret

irq_common_stub:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call IrqHandler
    pop ebx

    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    sti
    iret 

ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR 8
ISR_NO_ERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_NO_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_NO_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_NO_ERR 30
ISR_NO_ERR 31
ISR_NO_ERR 128
ISR_NO_ERR 177

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
