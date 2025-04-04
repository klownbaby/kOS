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

/* Define our global bs */
static fat16_bs_t bs;

/* Dump a directory entry, temporary */
static void
dump_dentry(dir_entry_t* dentry)
{
    epoch_date_t date = (epoch_date_t)dentry->date;
    char strname[9] = { 0 };

    // copy name to null terminated scratch buffer
    kstrncpy(strname, (const char*)dentry->name, 8);

    printk("%d ", dentry->size);
    printk("%d/%d/%d ", date.fields.day, date.fields.month, date.fields.year);
    printk("%s\n", strname);

}

/* Dump our BIOS parameter block to tty */
void
fat_dump_bs()
{
    void* first_sector = NULL;

    // allocate buffer of sector size
    first_sector = kmalloc(512);

    // zero that shit out just in case
    kmemset(first_sector, 0, 512);

    // read first sector into our buffer
    read_sectors(SLAVE_DRIVE, 1, 0, first_sector);

    kmemcpy(&bs, first_sector, sizeof(fat16_bs_t));

    printk("BIOS Parameter Block:\n");
    printk("    OEM name            -> %s\n", bs.oem_name);
    printk("    Sectors per cluster -> %d\n", bs.sectors_per_cluster);
    printk("    Bytes per sector    -> %d\n", bs.bytes_per_sector);
    printk("    Root entry count    -> %d\n", bs.root_entry_count);

    fat_dump_root();

    kfree(first_sector);
}

void
fat_dump_root()
{
    uint8_t* root_sector = NULL;
    uint32_t root_dir_lba = 0;
    uint32_t data_offset = 0;
    uint32_t next_dentry_offset = 0;
    dir_entry_t dentry = { 0 };
    char name_str[9] = { 0 };

    root_sector = kmalloc(512);

    // get root directory lba and offset of data sectors
    root_dir_lba = bs.reserved_sector_count + (bs.table_count * bs.table_size_16);
    data_offset = root_dir_lba + (bs.root_entry_count * 32);

    kmemset(root_sector, 0, 512);

    // read one sector at our root dir logical block address
    read_sectors(SLAVE_DRIVE, 1, root_dir_lba, root_sector);

    for (uint16_t i = 0; i < bs.root_entry_count; ++i)
    {   
        // get next offset
        next_dentry_offset = i * sizeof(dir_entry_t);

        // copy root dir into dentry buffer
        kmemcpy(&dentry, root_sector + next_dentry_offset, sizeof(dir_entry_t));

        if (dentry.attr == DIRECTORY)
        {
            dump_dentry(&dentry);
        }
    }

    // free our buffers
    kfree(root_sector);
}
