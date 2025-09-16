/**
 * 
 * @copyright Copyright (c) 2024, Kevin Kleiman, All Rights Reserved
 * 
 * This is the kernel for yet another hobbyOS designed and developed by Kevin Kleiman.
 * Feel free to copy, use, edit, etc. anything you see 
 *
 * This was originally designed to try writing a ring0 math library but I soon realized,
 * I should just make a full-blown kernel. It has been a great learning experience and I
 * implore anyone even remotely interested to fork, play around, contribute, whatever
 * you want. 
 *
 * For now, it's pretty barebones and shitty, but hopefully that will change with time.
 * Have fun creating kOS (pronounced "Chaos")
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Define some constats */
#define INTERRUPT_GATE       0x8E
#define INTERRUPT_USER_GATE  0xEE
#define TRAP_GATE            0x8F
#define TASK_GATE            0x85

#define PIC_CMD_PORT_MASTER  0x20
#define PIC_DATA_PORT_MASTER 0x21

#define PIC_CMD_PORT_SLAVE   0xA0
#define PIC_DATA_PORT_SLAVE  0xA1

#define MAX_INTERRUPTS       256

/* Alias IRQ indices */
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct _IDT_ENTRY {
	UINT16    isr_low;      
	UINT16    kernel_cs;    
	UINT8     reserved;    
	UINT8     attributes;   
	UINT16    isr_high;     
} __attribute__((packed)) IDT_ENTRY;

typedef struct _GATE_DESCRIPTOR {
   UINT16     offset_low;
   UINT16     selector;
   UINT8      zero;
   UINT8      type_attributes;
   UINT16     offset_high;
} __attribute__((packed)) GATE_DESCRIPTOR;

typedef struct _IDTR {
	UINT16	limit;
	ULONG	base;
} __attribute__((packed)) IDTR;

typedef struct {
   ULONG ds;
   ULONG edi, esi, ebp, esp, ebx, edx, ecx, eax;
   ULONG intNum, errCode;
   ULONG eip, cs, eflags, userEsp, ss;
} __attribute__((packed)) INTERRUPT_REGISTER_CONTEXT;

typedef VOID (*ISR)(INTERRUPT_REGISTER_CONTEXT);

/* Checks whether interrupts are enabled */
static inline bool __checkInterruptsEnabled() 
{
    ULONG flags;

    __asm__ __volatile__ ("pushf\n\t"
                           "pop %0"
                           : "=g"(flags));

    return flags & (1 << 9);
}

/* Assembly-defined extern for IDT load */
extern VOID 
LoadIdt(IDTR *);

/* Function definitions for idt setup and interrupts */
VOID
IdtInit(VOID);

VOID
RegisterInterruptHandler(UINT8 index, ISR handler);

VOID
IdtSetGate(UINT8 index, ULONG handler);

VOID
IsrHandler(INTERRUPT_REGISTER_CONTEXT irt_register);

/* ISRs */
extern VOID isr0();
extern VOID isr1();
extern VOID isr2();
extern VOID isr3();
extern VOID isr4();
extern VOID isr5();
extern VOID isr6();
extern VOID isr7();
extern VOID isr8();
extern VOID isr9();
extern VOID isr10();
extern VOID isr11();
extern VOID isr12();
extern VOID isr13();
extern VOID isr14();
extern VOID isr15();
extern VOID isr16();
extern VOID isr17();
extern VOID isr18();
extern VOID isr19();
extern VOID isr20();
extern VOID isr21();
extern VOID isr22();
extern VOID isr23();
extern VOID isr24();
extern VOID isr25();
extern VOID isr26();
extern VOID isr27();
extern VOID isr28();
extern VOID isr29();
extern VOID isr30();
extern VOID isr31();
extern VOID isr128();
extern VOID isr177();

/* IRQs */
extern VOID irq0();
extern VOID irq1();
extern VOID irq2();
extern VOID irq3();
extern VOID irq4();
extern VOID irq5();
extern VOID irq6();
extern VOID irq7();
extern VOID irq8();
extern VOID irq9();
extern VOID irq10();
extern VOID irq11();
extern VOID irq12();
extern VOID irq13();
extern VOID irq14();
extern VOID irq15();
