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
static fat_context_t fat_ctx;

/* Dump a directory entry, temporary */
static void
dump_dentry(dir_entry_t* dentry)
{
    char strname[9] = { 0 };
    epoch_date_t date = (epoch_date_t)dentry->crt_date;

    // copy name to null terminated scratch buffer
    kstrncpy(strname, (const char*)dentry->name, 8);

    printk("%d    ", dentry->size);
    printk("%d/%d/%d ", date.fields.day, date.fields.month, date.fields.year);
    printk("%s\n", strname);
}

/* Get next directory name from path */
static void
next_dir_name(char* path, char* outname)
{
    // copy until delimeter
    while (*path != '/' || *path != 0)
    {
        *outname++ = *path++;
    }
}

/* Read file into memory from path */
file_t
fat_open(char* path)
{
    file_t file = { 0 };
    uint32_t next_dentry_offset = 0;
    dir_entry_t dentry = { 0 };
    char strname[9] = { 0 };

    for (uint16_t i = 0; i < bs.root_entry_count; ++i)
    {   
        // get next offset
        next_dentry_offset = i * sizeof(dir_entry_t);

        kmemcpy(&dentry, fat_ctx.root_sector + next_dentry_offset, sizeof(dir_entry_t));

        if (dentry.attr == 0x0) continue;

        dump_dentry(&dentry);

        // copy name to null terminated scratch buffer
        kstrncpy(strname, (const char*)dentry.name, 8);

        if (kstrcmp(strname, path))
        {
            printk("Found file %s!\n", path);
            break;
        }
    }

    return file;
}

/* Dump our BIOS parameter block to tty */
void
fat_dump_bs()
{
    printk("BIOS Parameter Block:\n");
    printk("    OEM name            -> %s\n", bs.oem_name);
    printk("    Sectors per cluster -> %d\n", bs.sectors_per_cluster);
    printk("    Bytes per sector    -> %d\n", bs.bytes_per_sector);
    printk("    Root entry count    -> %d\n", bs.root_entry_count);
    printk("    Table size          -> %d\n", bs.table_size_16);
    printk("    Table count         -> %d\n", bs.table_count);
}

static void
init_bs()
{
    void* first_sector = NULL;

    // allocate buffer of sector size
    first_sector = kmalloc(512);
    kmemset(first_sector, 0, 512);

    // read first sector into our buffer
    read_sectors(SLAVE_DRIVE, 1, 0, first_sector);

    kmemcpy(&bs, first_sector, sizeof(fat16_bs_t));

    // finally, free our buffer
    kfree(first_sector);
}

void
fat_init()
{
    uint8_t* root_sector = NULL;
    uint8_t* data_sector = NULL;
    uint32_t root_dir_lba = 0;
    uint32_t data_lba = 0;
    uint32_t next_dentry_offset = 0;
    uint32_t n_root_sectors = 0;
    dir_entry_t dentry = { 0 };
    char name_str[9] = { 0 };

    // ensure our boot sector is initialized
    init_bs();

    // allocate and zero our root sector
    root_sector = kmalloc(512);
    kmemset(root_sector, 0, 512);

    // calculate number of root sectors
    n_root_sectors = ((bs.root_entry_count * 32) + (bs.bytes_per_sector - 1)) / bs.bytes_per_sector;

    // get root directory lba and offset of data sectors
    root_dir_lba = bs.reserved_sector_count + (bs.table_count * bs.table_size_16);
    data_lba = bs.reserved_sector_count + (bs.table_count * bs.table_size_16) + n_root_sectors;

    // alright dawg, now set our context
    fat_ctx.root_lba = root_dir_lba;
    fat_ctx.data_lba = data_lba;

    // allocate and zero our data sector
    data_sector = kmalloc(1024);
    kmemset(data_sector, 0, 1024);

    // read one sector at our root dir logical block address
    read_sectors(SLAVE_DRIVE, 1, root_dir_lba, root_sector);
    read_sectors(SLAVE_DRIVE, 1, data_lba, data_sector);
    read_sectors(SLAVE_DRIVE, 1, data_lba + 1, data_sector + 0x200);

    // set context buffers
    fat_ctx.root_sector = root_sector;

    // fat_open("dummy");
}
