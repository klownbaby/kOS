global LoadGdt

LoadGdt:
    mov eax, [esp+4]
    lgdt [eax]

    mov eax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.code

.code:
    ret
    
