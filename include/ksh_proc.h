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

VOID
HandleClear(ULONG argc, CHAR **argv);

VOID
HandleReboot(ULONG argc, CHAR **argv);

VOID
HandleDumpt(ULONG argc, CHAR **argv);

VOID
HandleDumpfs(ULONG argc, CHAR **argv);

VOID
HandleDumpfl(ULONG argc, CHAR **argv);

VOID
HandleNeofetch(ULONG argc, CHAR **argv);

VOID
HandlePoke(ULONG argc, CHAR **argv);

VOID
HandleProd(ULONG argc, CHAR **argv);

VOID
HandleLs(ULONG argc, CHAR **argv);

VOID
HandleCat(ULONG argc, CHAR **argv);
