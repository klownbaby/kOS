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

/* Define constants for conversions */
#define DECIMAL 10
#define HEX     16
#define BINARY  2

/* Standard I/O function definitions */
CHAR * 
KIToA(int value, CHAR *str, int base);

ULONG
KAToI(const CHAR *str);

VOID
KMemSet(VOID *dest, register int data, register SIZE length);

VOID
KMemCopy(VOID *dest, VOID *src, register SIZE size);

VOID 
KPanic(CHAR *msg);

VOID 
KPutS(const CHAR *str);

VOID 
KPrint(const CHAR *fmt, ...);
