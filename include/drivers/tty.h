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

#include <stddef.h>
#include <stdbool.h>
#include "vga.h"

#define BOOT_LOG(msg) \
    TTYWrite("[ "); \
    TTYWriteColor("OK", VGA_COLOR_GREEN, VGA_COLOR_BLACK); \
    TTYWrite(" ] "); \
    TTYWrite(msg); \
    TTYWrite("\n");

/* TTY function defs */
VOID 
TTYInit();

VOID 
TTYWrite(const CHAR* str);

VOID 
TTYWriteColor(const CHAR* str, VGA_COLOR fg, VGA_COLOR bg);

VOID 
TTYPutC(CHAR c);

VOID 
TTYPutCRelative(CHAR c, int relx, int rely, bool cursor);

VOID 
TTYClear();

VOID 
TTYSetColor(VGA_COLOR fg, VGA_COLOR bg);

VOID 
TTYNeofetch();
