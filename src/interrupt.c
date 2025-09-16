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

#include "kernel.h"
#include "drivers/tty.h"
#include "drivers/vga.h"

/* Create the actual IDT */
__attribute__((aligned(0x10))) static IDT_ENTRY idt[MAX_INTERRUPTS];

/* IDT register and interrupt handler table */
static IDTR idtr;
static ISR interruptHandlers[MAX_INTERRUPTS];

/* String value of exceptions from a given ISR */
static const CHAR* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check", 
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/* Initialize PIC */
static VOID 
picInit(VOID) 
{
    // setup master and slave PIC
    __outb(PIC_CMD_PORT_MASTER, 0x11);
    __outb(PIC_CMD_PORT_SLAVE, 0x11);

    // setup vector offsets
    __outb(PIC_DATA_PORT_MASTER, 0x20);
    __outb(PIC_DATA_PORT_SLAVE, 0x28);

    __outb(PIC_DATA_PORT_MASTER, 0x04);
    __outb(PIC_DATA_PORT_SLAVE, 0x02);

    __outb(PIC_DATA_PORT_MASTER, 0x01);
    __outb(PIC_DATA_PORT_SLAVE, 0x01);

    __outb(PIC_DATA_PORT_MASTER, 0xFF);
    __outb(PIC_DATA_PORT_SLAVE, 0xFF);

    // unmask IRQ1, IRQ0, and IRQ8
    __outb(PIC_DATA_PORT_MASTER, 0x7C);
}


VOID 
IdtInit(VOID) 
{
    // setup idt register data structure
    idtr.limit = (UINT16) (sizeof(idt) * 256) - 1;
    idtr.base = (ULONG) &idt;

    // setup ISR exception gates + syscalls
    IdtSetGate(0, (ULONG) isr0);
    IdtSetGate(1, (ULONG) isr1);
    IdtSetGate(2, (ULONG) isr2);
    IdtSetGate(3, (ULONG) isr3);
    IdtSetGate(4, (ULONG) isr4);
    IdtSetGate(5, (ULONG) isr5);
    IdtSetGate(6, (ULONG) isr6);
    IdtSetGate(7, (ULONG) isr7);
    IdtSetGate(8, (ULONG) isr8);
    IdtSetGate(9, (ULONG) isr9);
    IdtSetGate(10, (ULONG) isr10);
    IdtSetGate(11, (ULONG) isr11);
    IdtSetGate(12, (ULONG) isr12);
    IdtSetGate(13, (ULONG) isr13);
    IdtSetGate(14, (ULONG) isr14);
    IdtSetGate(15, (ULONG) isr15);
    IdtSetGate(16, (ULONG) isr16);
    IdtSetGate(17, (ULONG) isr17);
    IdtSetGate(18, (ULONG) isr18);
    IdtSetGate(19, (ULONG) isr19);
    IdtSetGate(20, (ULONG) isr20);
    IdtSetGate(21, (ULONG) isr21);
    IdtSetGate(22, (ULONG) isr22);
    IdtSetGate(23, (ULONG) isr23);
    IdtSetGate(24, (ULONG) isr24);
    IdtSetGate(25, (ULONG) isr25);
    IdtSetGate(26, (ULONG) isr26);
    IdtSetGate(27, (ULONG) isr27);
    IdtSetGate(28, (ULONG) isr28);
    IdtSetGate(29, (ULONG) isr29);
    IdtSetGate(30, (ULONG) isr30);
    IdtSetGate(31, (ULONG) isr31);
    IdtSetGate(128, (ULONG) isr128);
    IdtSetGate(177, (ULONG) isr177);

    // init programmable interrupt controller (PIC)
    picInit();

    // set IRQ gates
    IdtSetGate(32, (ULONG) irq0);
    IdtSetGate(33, (ULONG) irq1);
    IdtSetGate(34, (ULONG) irq2);
    IdtSetGate(35, (ULONG) irq3);
    IdtSetGate(36, (ULONG) irq4);
    IdtSetGate(37, (ULONG) irq5);
    IdtSetGate(38, (ULONG) irq6);
    IdtSetGate(39, (ULONG) irq7);
    IdtSetGate(40, (ULONG) irq8);
    IdtSetGate(41, (ULONG) irq9);
    IdtSetGate(42, (ULONG) irq10);
    IdtSetGate(43, (ULONG) irq11);
    IdtSetGate(44, (ULONG) irq12);
    IdtSetGate(45, (ULONG) irq13);
    IdtSetGate(46, (ULONG) irq14);
    IdtSetGate(47, (ULONG) irq15);

    // call extern idt load fucntion defined in __idt.S
    LoadIdt(&idtr);

    BOOT_LOG("IDT Loaded.")

    // check if interrupts are successfully enabled
    KASSERT_PANIC(!__checkInterruptsEnabled(), "Interrupt init fail!");
}

VOID 
RegisterInterruptHandler(UINT8 index, ISR handler) 
{
    // set interrupt handler entry in table
    interruptHandlers[index] = handler;
}

VOID 
IsrHandler(INTERRUPT_REGISTER_CONTEXT registers) 
{
    // handle ISR exceptions
    if (registers.intNum < 32) {
        // set text color to red for exceptions
        TTYSetColor(VGA_COLOR_RED, VGA_COLOR_BLACK);

        TTYWrite("\nKERNEL PANIC! ");
        TTYWrite(exception_messages[registers.intNum]);
            
        // stop execution
        __hlt();

    // handle syscall ISR
    } else if (registers.intNum == 128) {
        ISR handler = interruptHandlers[registers.intNum];

        handler(registers);
    }
}

VOID 
irq_handler(INTERRUPT_REGISTER_CONTEXT registers) 
{
    // call IRQ handler associated with interrupt number
    if (interruptHandlers[registers.intNum] != 0) {
        ISR handler = interruptHandlers[registers.intNum];
        handler(registers);
    }

    // send EOI instruction to PIC
    if (registers.intNum >= 40) __outb(PIC_CMD_PORT_SLAVE, 0x20);
    __outb(PIC_CMD_PORT_MASTER, 0x20);
}

VOID 
IdtSetGate(UINT8 index, ULONG handler) 
{
    // setup IDT gate, permissions, offset, etc.
    idt[index].isr_low = handler & 0xFFFF;
    idt[index].kernel_cs = 0x08;
    idt[index].reserved = 0;
    idt[index].isr_high = (handler >> 16) & 0xFFFF;

    if (index == 128) {
        idt[index].attributes = INTERRUPT_USER_GATE;
    } else {
        idt[index].attributes = INTERRUPT_GATE;
    }
}

MODULE_ENTRY_ORDERED(IdtInit, 2);

