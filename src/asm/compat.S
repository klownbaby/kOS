.global EnableCompatibilityMode

EFER_MSR equ 0xC0000080
EFER_LM_ENABLE equ 1 << 8

EnableCompatibilityMode:
    mov ecx, EFER_MSR
    rdmsr
    or eax, EFER_LM_ENABLE
    wrmsr
