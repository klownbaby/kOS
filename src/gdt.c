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

/* Declare our gdt buffer and entry pointer */
static GDT_ENTRY gdtEntries[GDT_ENTRIES];
static GDTR gdtPtr;

/* Sets an entry in the global descriptor table (GDT) */
VOID 
GdtSetEntry(
    ULONG index,
    ULONG base, 
    ULONG limit, 
    UINT8 access, 
    UINT8 granularity) 
{
    // set base fields accordingly
    gdtEntries[index].base_low = GDT_BASE_LOW(base);
    gdtEntries[index].base_mid = GDT_BASE_MID(base);
    gdtEntries[index].base_high = GDT_BASE_HIGH(base);

    // set base limit accordingly
    gdtEntries[index].limit = GDT_LIMIT(limit);
    gdtEntries[index].flags = GDT_FLAGS(limit);
    gdtEntries[index].flags |= GDT_GRANULARITY(granularity);
    
    // set access bits
    gdtEntries[index].access = access;
}

/* Init GDT by setting all segments and calling extern LoadGdt (asm) */
VOID 
GdtInit(VOID) 
{
    // set ou limit and base
    gdtPtr.limit = (sizeof(gdtEntries) * GDT_ENTRIES) - 1;
    gdtPtr.base = (GDT_ENTRY*)&gdtEntries;

    // set our default gdt entries, map all to kernel
    GdtSetEntry(0, 0, 0, 0, 0);
    GdtSetEntry(1, 0, GDT_HIGH_LIMIT, GDT_KERNEL_CODE, GDT_KERNEL_GRANULARITY);
    GdtSetEntry(2, 0, GDT_HIGH_LIMIT, GDT_KERNEL_DATA, GDT_KERNEL_GRANULARITY);
    GdtSetEntry(3, 0, GDT_HIGH_LIMIT, GDT_USER_CODE, GDT_USER_GRANULARITY);
    GdtSetEntry(4, 0, GDT_HIGH_LIMIT, GDT_USER_DATA, GDT_USER_GRANULARITY);

    // finally, load the gdt
    LoadGdt(&gdtPtr);

    BOOT_LOG("GDT Loaded.");
}

MODULE_ENTRY_ORDERED(GdtInit, 1);
