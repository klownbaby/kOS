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

#include <ktypes.h>
#include <stddef.h>

SIZE 
KStrLen(const CHAR *str);

VOID 
KStrCopy(CHAR *dest, const CHAR *src);

VOID 
KStrNCopy(CHAR *dest, const CHAR *src, SIZE n);

CHAR* 
KStrCat(CHAR *src, const CHAR *append);

ULONG
KStrNCmp(const CHAR *s1, const CHAR *s2, SIZE n);

ULONG
KStrNTok(CHAR *str, const CHAR delim);

ULONG
KStrTokOffset(CHAR *str, const CHAR delim);

CHAR **
KStrSplit(CHAR *str, const CHAR delim, ULONG *elemCount);
