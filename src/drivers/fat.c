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
#include "drivers/ata.h"

/* Define our global bpb */
static fat16_bpb_t* bpb;
static uint8_t* table;

/* Dump our BIOS parameter block to tty */
void
fat_dump_bpb()
{
    // allocate buffer of sector size
    bpb = kmalloc(512);

    // zero that shit out just in case
    kmemset(bpb, 0, 512);

    // read first sector into our buffer
    read_sectors(SLAVE_DRIVE, 1, 0, bpb);

    printk("BIOS Parameter Block:\n");
    printk("    OEM name            -> %s\n", bpb->oem_name);
    printk("    Sectors per cluster -> %d\n", bpb->sectors_per_cluster);
    printk("    Bytes per sector    -> %d\n", bpb->bytes_per_sector);

    // free our buffer
    kfree(bpb);
}
