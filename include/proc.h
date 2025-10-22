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
#include "ktypes.h"

/* Define base virtual address for all kprocesses */
#define KPROCESS_BASE        0x3fff0000

#define KPROCESS_CR3_VIRT    0x2acc0000

/* Define base virtual address for kprocess page directory list */
#define KPROCESS_PDLIST_BASE 0x8acc0000

KSTATUS
ProcLoad(PROC_HANDLE *handle);

KSTATUS
ProcExec(PROC_HANDLE *handle);
