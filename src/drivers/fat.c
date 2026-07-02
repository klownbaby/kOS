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
    dataSize = (numClusters * bs.sectorsPerCluster) * bs.bytesPerSector;

    // allocate minimum-sized buffer (aligned to sector size)
    data = KMalloc(dataSize);
    KMemSet(data, 0, dataSize);

    for (ULONG i = 0; i < numClusters; ++i)
    {
        // get LBA of cluster
        clusterLba = CLUSTER_TO_LBA(cluster);

        // get offset into data buffer
        dataOffset = (i * bs.sectorsPerCluster) * bs.bytesPerSector;

        // sectors into data buffer at offset
        AtaReadSectors(
             SLAVE_DRIVE,
             bs.sectorsPerCluster,
             clusterLba,
             data + dataOffset);

        cluster = fatCtx.fatSector[cluster];

        if (cluster >= 0xfff8) break;
    }

    // CALLER owns this now!
    return data;
}

static BOOLEAN
compareName(CHAR *name, DIR_ENTRY *dentry)
{
    BOOLEAN match = FALSE;
    CHAR strname[9] = { 0 };

    // for now, not supporting LFNs
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

/* Find file or directory entry in directory blob */
static VOID *
findEntryInDirecotry(CHAR **path, ULONG *outSize, ULONG next)
{
    VOID *data = NULL;
    CHAR strname[9] = { 0 };
    DIR_ENTRY dentry = { 0 };
    ULONG nextDentryOffset = 0;

    if (next > 0)
    {
        data = findEntryInDirecotry(path, outSize, (next - 1));
        KASSERT_GOTO_FAIL(data == NULL);
    }
    else
    {
        data = fatCtx.rootSector;
    }

    // copy first dentry
    KMemCopy(&dentry, data, sizeof(DIR_ENTRY));

    while (dentry.name[0] != 0)
    {   
        // get next offset
        nextDentryOffset += sizeof(DIR_ENTRY);

        // copy that bitch over
        KMemCopy(&dentry, data + nextDentryOffset, sizeof(DIR_ENTRY));

        // only caring about directories and files for now
        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        if (compareName(path[next], &dentry))
        {
            // free previous dir buffer
            if (data != fatCtx.rootSector)
            {
                KFree(data);
            }

            data = readAllClusters(dentry.lowCluster, dentry.size);
            *outSize = dentry.size;

            GOTO_SUCCESS;
        }
    }
   
fail:
    data = NULL;
    *outSize = 0;

success:
    return data;
}

/* Read file into memory (only from root dir for the moment) */
VOID *
FatOpen(CHAR *path, ULONG *outSize)
{
    VOID *data = NULL;
    CHAR **pathElems = NULL;
    ULONG numDirs = 0;

    pathElems = KStrSplit(path, '/', &numDirs);

    data = findEntryInDirecotry(pathElems, outSize, (numDirs - 1));

    if (pathElems)
    {
        KFree(pathElems);
    }

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
FatDumpDirectory(VOID *buffer, ULONG size)
{
    DIR_ENTRY dentry = { 0 };
    ULONG nextDentryOffset = 0;

    while (dentry.name[0] != 0)
    {
        // get next offset
        nextDentryOffset += sizeof(DIR_ENTRY);

        KPrint("Attempting to dump dentry..\n");

        // copy that bitch over
        KMemCopy(&dentry, buffer + nextDentryOffset, sizeof(DIR_ENTRY));

        if (dentry.attr != DIRECTORY && dentry.attr != FILE) continue;

        // dump each entry
        FatDumpDentry(&dentry);
    }
}

/* Dump root directory */
VOID
FatDumpRoot(VOID)
{
    ULONG nextDentryOffset = 0;
    DIR_ENTRY dentry = { 0 };
    CHAR strname[9] = { 0 };
    ULONG rootEntries = 0;

    // calculate number of root directory entries
    rootEntries = ((bs.rootEntryCount * 32) +
                   (bs.bytesPerSector - 1)) / bs.bytesPerSector;
    
    for (UINT16 i = 0; i < rootEntries; ++i)
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

/* Dump our BIOS parameter block to TTY */
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

    // calculate number of root sectors
    numRootSectors =
        ((bs.rootEntryCount * 32) + (bs.bytesPerSector - 1)) / bs.bytesPerSector;

    // allocate and zero our root sector
    fatCtx.rootSector = KMalloc(numRootSectors * bs.bytesPerSector);
    KMemSet(fatCtx.rootSector, 0, (numRootSectors * bs.bytesPerSector));

    // get root directory lba and offset of data sectors
    fatCtx.rootLba =
        bs.reservedSectorCount + (bs.tableCount * bs.tableSize16);
    fatCtx.dataLba =
        bs.reservedSectorCount + (bs.tableCount * bs.tableSize16) + numRootSectors;
    fatCtx.fatLba = bs.reservedSectorCount;

    ULONG fatSectSize = (bs.tableCount * bs.tableSize16);
    fatSectSize = ((fatSectSize + (bs.bytesPerSector - 1)) / bs.bytesPerSector) * bs.bytesPerSector;

    // allocate and zero our data sector
    fatCtx.fatSector = KMalloc(fatSectSize);
    KMemSet(fatCtx.fatSector, 0, fatSectSize);

    // read one sector at our root dir logical block address
    AtaReadSectors(SLAVE_DRIVE, numRootSectors, fatCtx.rootLba, fatCtx.rootSector);
    AtaReadSectors(SLAVE_DRIVE, (fatSectSize / 512), fatCtx.fatLba, fatCtx.fatSector);
}

MODULE_ENTRY(FatInit);
