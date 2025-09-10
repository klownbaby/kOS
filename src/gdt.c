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
static gdt_entry_t gdt_entries[GDT_ENTRIES];
static gdtr_t gdt_ptr;

/* Declare our tss entry buffer */
static tss_entry_t tss_entry;

/* Set our kernel stack pointer in TSS */
void
tss_set_kernel_sp(uint32_t sp)
{
    tss_entry.esp0 = sp;
}

/* Sets an entry in the global descriptor table (GDT) */
void 
gdt_set_entry(
    uint32_t index,
    uint32_t base, 
    uint32_t limit, 
    uint8_t access, 
    uint8_t granularity) 
{
    // set base fields accordingly
    gdt_entries[index].base_low = GDT_BASE_LOW(base);
    gdt_entries[index].base_mid = GDT_BASE_MID(base);
    gdt_entries[index].base_high = GDT_BASE_HIGH(base);

    // set base limit accordingly
    gdt_entries[index].limit = GDT_LIMIT(limit);
    gdt_entries[index].flags = GDT_FLAGS(limit);
    gdt_entries[index].flags |= GDT_GRANULARITY(granularity);
    
    // set access bits
    gdt_entries[index].access = access;
}

/* Init GDT by setting all segments and calling extern load_gdt (asm) */
void 
gdt_init() 
{
    // set ou limit and base
    gdt_ptr.limit = (sizeof(gdt_entries) * GDT_ENTRIES) - 1;
    gdt_ptr.base = (gdt_entry_t*)&gdt_entries;

    // set our default gdt entries, map all to kernel
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, GDT_HIGH_LIMIT, GDT_KERNEL_CODE, GDT_KERNEL_GRANULARITY);
    gdt_set_entry(2, 0, GDT_HIGH_LIMIT, GDT_KERNEL_DATA, GDT_KERNEL_GRANULARITY);
    gdt_set_entry(3, 0, GDT_HIGH_LIMIT, GDT_USER_CODE, GDT_USER_GRANULARITY);
    gdt_set_entry(4, 0, GDT_HIGH_LIMIT, GDT_USER_DATA, GDT_USER_GRANULARITY);
    // set our tss entry
    gdt_set_entry(
        5, (uint32_t)&tss_entry, 
        sizeof(tss_entry) - 1, 
        GDT_TSS, 
        0);

    tss_entry.ss0 = (uint32_t)&_data_start;

    // finally, load the gdt
    load_gdt(&gdt_ptr);

    BOOT_LOG("GDT Loaded.");
}

MODULE_ENTRY_ORDERED(gdt_init, 1);
