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

#pragma once

#include "ktypes.h"

/* Define FAT attribute types */
#define VOLUME    0x8
#define DIRECTORY 0x10
#define FILE      0x20

#define ATTR_TYPE_TO_STR(__type) \
    __type == 0x10 ? "dir" : "file"

#define CLUSTER_TO_LBA(__cluster) \
    (fatCtx.dataLba + ((__cluster - 2) * bs.sectorsPerCluster))

typedef struct _FAT_CONTEXT {
    /* Common LBAs */
    ULONG  fatLba;
    ULONG  rootLba;
    ULONG  dataLba;

    UINT16 *fatSector;
    UINT8  *rootSector;
    UINT8  *dataSector;
} FAT_CONTEXT;

/* Define FAT16 BIOS parameter block */
typedef struct _FAT16_BS {
  UINT8  bootJmp[3];
	UINT8  oemName[8];
	UINT16 bytesPerSector;
	UINT8	 sectorsPerCluster;
	UINT16 reservedSectorCount;
	UINT8  tableCount;
	UINT16 rootEntryCount;
	UINT16 totalSectors16;
	UINT8	 mediaType;
	UINT16 tableSize16;
	UINT16 sectorsPerTrack;
	UINT16 headSideCount;
	ULONG  hiddenSectorCount;
	ULONG  totalSectors32;
  UINT8	 biosDriveNum;
	UINT8	 reserved1;
	UINT8	 bootSignature;
	ULONG  volumeId;
	UINT8	 volumeLabel[11];
	UINT8	 fatTypeLabel[8];
} __attribute__((packed)) FAT16_BS;

typedef struct _DIR_ENTRY {
    UINT8  name[8];
    UINT8  ext[3];
    UINT8  attr;
    UINT8  reserved;
    UINT8  time[3];
    UINT16 crtDate;
    UINT16 accDate;
    UINT16 highCluster;
    UINT8  writtenDate[4];
    UINT16 lowCluster;
    ULONG  size;
} DIR_ENTRY;

/* FAT function definitions */
VOID
FatInit(VOID);

VOID
FatDumpBs(VOID);

VOID
FatDumpDentry(DIR_ENTRY *dentry);

VOID
FatDumpDirectory(VOID *buffer);

VOID
FatDumpRoot(VOID);

VOID *
FatOpen(CHAR *path, ULONG *outSize);
