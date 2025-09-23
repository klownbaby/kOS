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
    numClusters = ((size / bs.sectorsPerCluster) / 512) + 1;
    dataSize = (numClusters * bs.sectorsPerCluster) * 512;

    // allocate minimum-sized buffer (aligned to sector size)
    data = KMalloc(dataSize);

    // get LBA of cluster
    clusterLba = CLUSTER_TO_LBA(cluster);

    for (ULONG i = 0; i < numClusters; ++i)
    {
        // get offset into data buffer
        dataOffset = (i * bs.sectorsPerCluster) * 512;

        // sectors into data buffer at offset
        AtaReadSectors(
             SLAVE_DRIVE,
             bs.sectorsPerCluster,
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

    for (UINT16 i = 0; i < bs.rootEntryCount; ++i)
    {   
        // get next offset
        nextDentryOffset = i * sizeof(DIR_ENTRY);

        // copy that bitch over
        KMemCopy(&dentry, fatCtx.rootSector + nextDentryOffset, sizeof(DIR_ENTRY));

        // only caring about directories and files for now
        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        if (compareName(path, &dentry))
        {
            data = readAllClusters(dentry.lowCluster, dentry.size);
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
    EPOCH_DATE date = (EPOCH_DATE)dentry->crtDate;

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

    for (UINT16 i = 0; i < bs.rootEntryCount; ++i)
    {   
        // get next offset
        nextDentryOffset = i * sizeof(DIR_ENTRY);

        // copy that bitch over
        KMemCopy(&dentry, fatCtx.rootSector + nextDentryOffset, sizeof(DIR_ENTRY));

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
    KPrint("    OEM name            -> %s\n", bs.oemName);
    KPrint("    Sectors per cluster -> %d\n", bs.sectorsPerCluster);
    KPrint("    Bytes per sector    -> %d\n", bs.bytesPerSector);
    KPrint("    Root entry count    -> %d\n", bs.rootEntryCount);
    KPrint("    Table size          -> %d\n", bs.tableSize16);
    KPrint("    Table count         -> %d\n", bs.tableCount);
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
    fatCtx.rootSector = KMalloc(512);
    KMemSet(fatCtx.rootSector, 0, 512);

    // calculate number of root sectors
    numRootSectors =
        ((bs.rootEntryCount * 32) + (bs.bytesPerSector - 1)) / bs.bytesPerSector;

    // get root directory lba and offset of data sectors
    fatCtx.rootLba =
        bs.reservedSectorCount + (bs.tableCount * bs.tableSize16);
    fatCtx.dataLba =
        fatCtx.rootLba + numRootSectors;
    fatCtx.fatLba = bs.reservedSectorCount;

    // allocate and zero our data sector
    fatCtx.dataSector = KMalloc(1024);
    KMemSet(fatCtx.dataSector, 0, 1024);

    // read one sector at our root dir logical block address
    AtaReadSectors(SLAVE_DRIVE, 1, fatCtx.rootLba, fatCtx.rootSector);
    AtaReadSectors(SLAVE_DRIVE, 1, fatCtx.dataLba, fatCtx.dataSector);
    AtaReadSectors(SLAVE_DRIVE, 1, fatCtx.fatLba, fatCtx.fatSector);
}

MODULE_ENTRY(FatInit);
