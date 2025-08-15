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

#define KBRD_INTRFC 0x64
#define KBRD_RESET 0xFE

/*

Contains all I/O functions (some inline for optimization)

Some other useful assembly mappings to clear interrupts, set interrupts,
and halt cpu execution which are useful in many branches

*/

/* Read a byte from a word-sized port */
static inline uint8_t inb(uint16_t port) 
{
    uint8_t ret;

    __asm__ __volatile__( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory" );

    return ret;
}

/* Read a word from a word-sized port */
static inline uint16_t inw(uint16_t port) 
{
    uint16_t ret;

    __asm__ __volatile__( "inw %1, %0" : "=a"(ret) : "Nd"(port));

    return ret;
}

/* Write a byte to a word-sized port */
static inline void outb(uint16_t port, uint8_t byte) 
{
    __asm__ __volatile__( "outb %0, %1" : : "a"(byte), "Nd"(port) : "memory" );
}

/* Clear interrupts */
__attribute__((used)) static void cli() 
{
    __asm__ __volatile__( "cli" );
}

/* Set (re-enable) interrupts */
__attribute__((used)) static void sti() 
{
    __asm__ __volatile__( "sti" );
}

/* Halt all CPU execution and loop infinitely */
__attribute__((noreturn, used)) static void hlt() 
{
    __asm__ __volatile__( "hlt" );

    for(;;);
}

__attribute__((noreturn, used)) static void warm_reboot()
{
    // send cpu reset signal
    outb(KBRD_INTRFC, KBRD_RESET);

    // halt all execution
    hlt();
}
