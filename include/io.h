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

#include "ktypes.h"

/* Define reboot contants */
#define KBRD_INTRFC 0x64
#define KBRD_RESET 0xFE

/* Read a byte from a word-sized port */
static inline UINT8 __inb(UINT16 port) 
{
    UINT8 ret;

    __asm__ __volatile__( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory" );

    return ret;
}

/* Read a word from a word-sized port */
static inline UINT16 __inw(UINT16 port) 
{
    UINT16 ret;

    __asm__ __volatile__( "inw %1, %0" : "=a"(ret) : "Nd"(port));

    return ret;
}

/* Write a byte to a word-sized port */
static inline VOID __outb(UINT16 port, UINT8 byte) 
{
    __asm__ __volatile__( "outb %0, %1" : : "a"(byte), "Nd"(port) : "memory" );
}

/* Clear interrupts */
static inline VOID __cli(VOID) 
{
    __asm__ __volatile__( "cli" );
}

/* Set (re-enable) interrupts */
static inline VOID __sti(VOID) 
{
    __asm__ __volatile__( "sti" );
}

/* Halt all CPU execution and loop infinitely */
__attribute__((noreturn)) static inline VOID __hlt(VOID) 
{
    __asm__ __volatile__( "hlt" );

    for(;;);
}

__attribute__((noreturn)) static VOID __warmReboot(VOID)
{
    // send cpu reset signal
    __outb(KBRD_INTRFC, KBRD_RESET);

    // halt all execution
    __hlt();
}
