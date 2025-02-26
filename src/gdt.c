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

static gdt_entry_t gdt_entries[GDT_ENTRIES];
static gdtr_t gdt_ptr;

/* Sets an entry in the global descriptor table (GDT) */
static void 
__set_gdt_entry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) 
{
    gdt_entries[index].base_low = GDT_BASE_LOW(base);
    gdt_entries[index].base_mid = GDT_BASE_MID(base);
    gdt_entries[index].base_high = GDT_BASE_HIGH(base);

    gdt_entries[index].limit = GDT_LIMIT(limit);
    gdt_entries[index].flags = GDT_FLAGS(limit);
    gdt_entries[index].flags |= GDT_GRANULARITY(granularity);
    
    gdt_entries[index].access = access;
}

/* Init GDT by setting all segments and calling extern load_gdt (asm) */
void 
gdt_init() 
{
    gdt_ptr.limit = (sizeof(gdt_entries) * GDT_ENTRIES) - 1;
    gdt_ptr.base = &gdt_entries;

    __set_gdt_entry(0, 0, 0, 0, 0);
    __set_gdt_entry(1, 0, GDT_HIGH_LIMIT, GDT_KERNEL_CODE, GDT_KERNEL_GRANULARITY);
    __set_gdt_entry(2, 0, GDT_HIGH_LIMIT, GDT_KERNEL_DATA, GDT_KERNEL_GRANULARITY);
    __set_gdt_entry(3, 0, GDT_HIGH_LIMIT, 0x03, GDT_KERNEL_GRANULARITY);
    __set_gdt_entry(4, 0, GDT_HIGH_LIMIT, 0x03, GDT_KERNEL_GRANULARITY);

    load_gdt(&gdt_ptr);

    BOOT_LOG("GDT Loaded.");
}

