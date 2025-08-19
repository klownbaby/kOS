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
#include "drivers/tty.h"

/* Define our global bs */
static fat16_bs_t bs;
static fat_context_t fat_ctx;

/* Allocate buffer for clusters, read all data into buffer */
static void *
read_all_clusters(uint32_t cluster, uint32_t size)
{
    uint8_t *data = NULL;
    uint32_t data_offset = 0;
    uint32_t cluster_lba = 0;
    uint32_t nclusters = 0;

    // ahhh this is shitty, works for now (minimum of one cluster)
    nclusters = ((size / bs.sectors_per_cluster) / 512) + 1;

    // allocate minimum-sized buffer (aligned to sector size)
    data = kmalloc(nclusters * 512);
    // get LBA of cluster
    cluster_lba = CLUSTER_TO_LBA(cluster);

    for (uint32_t i = 0; i < nclusters; ++i)
    {
        // get offset into data buffer
        data_offset = (i * bs.sectors_per_cluster) * 512;

        // sectors into data buffer at offset
        read_sectors(
             SLAVE_DRIVE,
             bs.sectors_per_cluster,
             cluster_lba,
             data + data_offset);
    }

    // CALLER owns this now!
    return data;
}

/* Dump a directory entry, temporary */
static void
dump_dentry(dir_entry_t* dentry)
{
    // align dynamic length strings to column
    char strname[9] = { ' ' };
    epoch_date_t date = (epoch_date_t)dentry->crt_date;

    // copy name to null terminated scratch buffer
    kstrncpy(strname, (const char*)dentry->name, 8);

    // dump that JAWN
    printk("%d/%d/%d ", date.fields.day, date.fields.month, date.fields.year);

    // oooo fancy color shit!
    if (dentry->attr == DIRECTORY)
    {
        tty_writecolor(strname, VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    } else {
        printk("%s", strname);
    }

    printk("%d\n", dentry->size);

    if (dentry->attr == DIRECTORY)
    {
        printk("\n");
        printk("dentry cluster offset 0x%x\n", CLUSTER_TO_LBA(dentry->low_cluster) * 512);

        dir_entry_t *test = read_all_clusters(dentry->low_cluster, dentry->size);
        printk("next dentry size 0x%x\n", test[1].size);

        kfree(test);

        printk("\n");
    }
}

static bool
compare_name(char *name, dir_entry_t *dentry)
{
    bool match = FALSE;
    char strname[9] = { 0 };

    kstrncpy(strname, (const char *)dentry->name, 8);

    if (kstrncmp(name, strname, kstrlen(name)) == 0)
    {
        match = TRUE;
    }

    return match;
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


/* Read file into memory (only from root dir for the moment) */
void *
fat_open(char *path, uint32_t *outsize)
{
    void *data = NULL;
    uint32_t next_dentry_offset = 0;
    dir_entry_t dentry = { 0 };
    char strname[9] = { 0 };

    for (uint16_t i = 0; i < bs.root_entry_count; ++i)
    {   
        // get next offset
        next_dentry_offset = i * sizeof(dir_entry_t);

        // copy that bitch over
        kmemcpy(&dentry, fat_ctx.root_sector + next_dentry_offset, sizeof(dir_entry_t));

        // only caring about directories and files for now
        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        if (compare_name(path, &dentry))
        {
            data = read_all_clusters(dentry.low_cluster, dentry.size);
            break;
        }
    }

    *outsize = dentry.size;

    return data;
}

/* Dump root directory */
void
fat_dump_root()
{
    uint32_t next_dentry_offset = 0;
    dir_entry_t dentry = { 0 };
    char strname[9] = { 0 };

    for (uint16_t i = 0; i < bs.root_entry_count; ++i)
    {   
        // get next offset
        next_dentry_offset = i * sizeof(dir_entry_t);

        // copy that bitch over
        kmemcpy(&dentry, fat_ctx.root_sector + next_dentry_offset, sizeof(dir_entry_t));

        // only caring about directories and files for now
        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        dump_dentry(&dentry);
    }
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

void
fat_init()
{
    uint32_t next_dentry_offset = 0;
    uint32_t n_root_sectors = 0;
    dir_entry_t dentry = { 0 };

    // ensure our boot sector is initialized
    init_bs();

    // allocate and zero our root sector
    fat_ctx.root_sector = kmalloc(512);
    kmemset(fat_ctx.root_sector, 0, 512);

    // calculate number of root sectors
    n_root_sectors =
        ((bs.root_entry_count * 32) + (bs.bytes_per_sector - 1)) / bs.bytes_per_sector;

    // get root directory lba and offset of data sectors
    fat_ctx.root_lba =
        bs.reserved_sector_count + (bs.table_count * bs.table_size_16);
    fat_ctx.data_lba =
        fat_ctx.root_lba + n_root_sectors;
    fat_ctx.fat_lba = bs.reserved_sector_count;

    // allocate and zero our data sector
    fat_ctx.data_sector = kmalloc(1024);
    kmemset(fat_ctx.data_sector, 0, 1024);

    // read one sector at our root dir logical block address
    read_sectors(SLAVE_DRIVE, 1, fat_ctx.root_lba, fat_ctx.root_sector);
    read_sectors(SLAVE_DRIVE, 1, fat_ctx.data_lba, fat_ctx.data_sector);
    read_sectors(SLAVE_DRIVE, 1, fat_ctx.fat_lba, fat_ctx.fat_sector);
}
