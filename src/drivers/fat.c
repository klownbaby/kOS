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
static FAT16_BS bs;
static FAT_CONTEXT fatCtx;

/* Allocate buffer for clusters, read all data into buffer */
static VOID *
readAllClusters(ULONG cluster, ULONG size)
{
    UINT8 *data = NULL;
    ULONG dataOffset = 0;
    ULONG dataSize = 0;
    ULONG clusterLba = 0;
    ULONG numClusters = 0;

    // ahhh this is shitty, works for now (minimum of one cluster)
    numClusters = ((size / bs.sectors_per_cluster) / 512) + 1;
    dataSize = (numClusters * bs.sectors_per_cluster) * 512;

    // allocate minimum-sized buffer (aligned to sector size)
    data = KMalloc(dataSize);

    // ensure we zero-out our buffer
    KMemSet(data, 0, dataSize);

    // get LBA of cluster
    clusterLba = CLUSTER_TO_LBA(cluster);

    for (ULONG i = 0; i < numClusters; ++i)
    {
        // get offset into data buffer
        dataOffset = (i * bs.sectors_per_cluster) * 512;

        // sectors into data buffer at offset
        AtaReadSectors(
             SLAVE_DRIVE,
             bs.sectors_per_cluster,
             clusterLba,
             data + dataOffset);
    }

    // CALLER owns this now!
    return data;
}

static BOOLEAN
compareName(CHAR *name, DIR_ENTRY *dentry)
{
    BOOLEAN match = FALSE;
    CHAR strname[9] = { 0 };

    KStrNCopy(strname, (const CHAR *)dentry->name, 8);

    if (KStrNCmp(name, strname, KStrLen(name)) == 0)
    {
        match = TRUE;
    }

    return match;
}

static VOID
initBs(VOID)
{
    VOID *firstSector = NULL;

    // allocate buffer of sector size
    firstSector = KMalloc(512);
    KMemSet(firstSector, 0, 512);

    // read first sector into our buffer
    AtaReadSectors(SLAVE_DRIVE, 1, 0, firstSector);

    KMemCopy(&bs, firstSector, sizeof(FAT16_BS));

    // finally, free our buffer
    KFree(firstSector);
}

/* Read file into memory (only from root dir for the moment) */
VOID *
FatOpen(CHAR *path, ULONG *outsize)
{
    VOID *data = NULL;
    DIR_ENTRY dentry = { 0 };
    ULONG nextDentryOffset = 0;
    CHAR strname[9] = { 0 };

    for (UINT16 i = 0; i < bs.root_entry_count; ++i)
    {   
        // get next offset
        nextDentryOffset = i * sizeof(DIR_ENTRY);

        // copy that bitch over
        KMemCopy(&dentry, fatCtx.root_sector + nextDentryOffset, sizeof(DIR_ENTRY));

        // only caring about directories and files for now
        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        if (compareName(path, &dentry))
        {
            data = readAllClusters(dentry.low_cluster, dentry.size);
            break;
        }
    }

    *outsize = dentry.size;

    return data;
}

/* Dump a directory entry, temporary */
VOID
FatDumpDentry(DIR_ENTRY *dentry)
{
    // align dynamic length strings to column
    CHAR strname[9] = { ' ' };
    EPOCH_DATE date = (EPOCH_DATE)dentry->crt_date;

    // copy name to null terminated scratch buffer
    KStrNCopy(strname, (const CHAR*)dentry->name, 8);

    // dump that JAWN
    KPrint("%d/%d/%d ", date.fields.day, date.fields.month, date.fields.year);

    // oooo fancy color shit!
    if (dentry->attr == DIRECTORY)
    {
        TTYWriteColor(strname, VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    } else {
        KPrint("%s", strname);
    }

    KPrint("%d\n", dentry->size);
}

/* Dump all contents of a buffer as if it were a directory */
VOID
FatDumpDirectory(VOID *buffer)
{
    ULONG dentry = (ULONG)buffer;

    while (((DIR_ENTRY *)dentry)->attr != 0)
    {
        // dump each entry
        FatDumpDentry((DIR_ENTRY *)dentry);

        // iterate
        dentry += sizeof(DIR_ENTRY);
    }
}

/* Dump root directory */
VOID
FatDumpRoot(VOID)
{
    ULONG nextDentryOffset = 0;
    DIR_ENTRY dentry = { 0 };
    CHAR strname[9] = { 0 };

    for (UINT16 i = 0; i < bs.root_entry_count; ++i)
    {   
        // get next offset
        nextDentryOffset = i * sizeof(DIR_ENTRY);

        // copy that bitch over
        KMemCopy(&dentry, fatCtx.root_sector + nextDentryOffset, sizeof(DIR_ENTRY));

        // only caring about directories and files for now
        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        FatDumpDentry(&dentry);
    }
}

/* Dump our BIOS parameter block to tty */
VOID
FatDumpBs(VOID)
{
    KPrint("BIOS Parameter Block:\n");
    KPrint("    OEM name            -> %s\n", bs.oem_name);
    KPrint("    Sectors per cluster -> %d\n", bs.sectors_per_cluster);
    KPrint("    Bytes per sector    -> %d\n", bs.bytes_per_sector);
    KPrint("    Root entry count    -> %d\n", bs.root_entry_count);
    KPrint("    Table size          -> %d\n", bs.table_size_16);
    KPrint("    Table count         -> %d\n", bs.table_count);
}

/* Initialize FAT filesystem, duh */
VOID
FatInit(VOID)
{
    ULONG nextDentryOffset = 0;
    ULONG numRootSectors = 0;
    DIR_ENTRY dentry = { 0 };

    // ensure our boot sector is initialized
    initBs();

    // allocate and zero our root sector
    fatCtx.root_sector = KMalloc(512);
    KMemSet(fatCtx.root_sector, 0, 512);

    // calculate number of root sectors
    numRootSectors =
        ((bs.root_entry_count * 32) + (bs.bytes_per_sector - 1)) / bs.bytes_per_sector;

    // get root directory lba and offset of data sectors
    fatCtx.root_lba =
        bs.reserved_sector_count + (bs.table_count * bs.table_size_16);
    fatCtx.data_lba =
        fatCtx.root_lba + numRootSectors;
    fatCtx.fat_lba = bs.reserved_sector_count;

    // allocate and zero our data sector
    fatCtx.data_sector = KMalloc(1024);
    KMemSet(fatCtx.data_sector, 0, 1024);

    // read one sector at our root dir logical block address
    AtaReadSectors(SLAVE_DRIVE, 1, fatCtx.root_lba, fatCtx.root_sector);
    AtaReadSectors(SLAVE_DRIVE, 1, fatCtx.data_lba, fatCtx.data_sector);
    AtaReadSectors(SLAVE_DRIVE, 1, fatCtx.fat_lba, fatCtx.fat_sector);
}

MODULE_ENTRY(FatInit);
