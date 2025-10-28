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

#include "hash.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "kmalloc.h"
#include "kutils.h"
#include "pmm.h"
#include "stdio.h"
#include "string.h"
#include "ksh.h"
#include "proc.h"
#include "exec.h"
#include "pe.h"
#include "syms.h"

/* Module entry point macro for unordered initialization */
#define MODULE_ENTRY(__entry) \
    __attribute__((section(".dynmod"))) MODULE_ENTRY *__entry##_dynmod = \
        (MODULE_ENTRY *)__entry

/* Module entry point macro for ordered initialization */
#define MODULE_ENTRY_ORDERED(__entry, __n) \
    __attribute__((section(".dynmod.init"#__n))) MODULE_ENTRY *__entry##_dynmod = \
        (MODULE_ENTRY *)__entry

/* Kernel start/end mappings from linker */
extern volatile ULONG _kernel_start;
extern volatile ULONG _kernel_end;

/* Kernel data section from linker */
extern volatile ULONG _data_start;

/* Module entry point region */
extern volatile ULONG _dynmod_start;
extern volatile ULONG _dynmod_end;

/* Kernel page directory */
extern volatile ULONG g_KernelPageDir[PD_NENTRIES];

/* GRUB multiboot info from handoff to kernel */
extern MULTIBOOT_INFO *g_Mbd;

/* Global console TTY state */
extern TTY_STATE g_TTYState;

/* Number of enabled cores */
extern ULONG g_NumCores;

/* Saving some time for commonly used linker-defined globals */
extern ULONG g_KernelStart;
extern ULONG g_KernelEnd;

/* Kernel heap */
extern UINT8 *g_HeapStart;
extern UINT8 *g_HeapEnd;

/* Global kernel symbol table, manually setting 100 entries for now */
extern KSYMBOL g_KernelSyms[100];
