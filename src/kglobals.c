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
#include "drivers/fat.h"

 /* Number of enabled cores */
ULONG g_NumCores = 0;

/* GRUB multiboot info from handoff to kernel */
MULTIBOOT_INFO *g_Mbd = NULL;

/* Global console TTY state */
TTY_STATE g_TTYState = { 0 };

/* Kernel start and end globals */
ULONG g_KernelStart = 0;
ULONG g_KernelEnd = 0;

/* Heap globals */
UINT8 *g_HeapStart = (UINT8 *)0x7ff00000;
UINT8 *g_HeapEnd = 0;

/* Global kernel symbol table, manually setting 100 entries for now */
KSYMBOL g_KernelSyms[100] = {
    DEFINE_SYMBOL(KIToA),
    DEFINE_SYMBOL(KAToI),
    DEFINE_SYMBOL(KMemSet),
    DEFINE_SYMBOL(KMemCopy),
    DEFINE_SYMBOL(KPanic),
    DEFINE_SYMBOL(KPutS),
    DEFINE_SYMBOL(KPrint),
    DEFINE_SYMBOL(DumpFreeList),
    DEFINE_SYMBOL(KSbrk),
    DEFINE_SYMBOL(KMalloc),
    DEFINE_SYMBOL(KFree),
    DEFINE_SYMBOL(FatDumpBs),
    DEFINE_SYMBOL(FatDumpDentry),
    DEFINE_SYMBOL(FatDumpRoot),
    DEFINE_SYMBOL(FatOpen),
};
