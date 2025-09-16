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

#include <stdint.h>
#include "ktypes.h"

/* Define FAT attribute types */
#define VOLUME    0x8
#define DIRECTORY 0x10
#define FILE      0x20

#define ATTR_TYPE_TO_STR(__type) \
    __type == 0x10 ? "dir" : "file"

#define CLUSTER_TO_LBA(__cluster) \
    (fatCtx.data_lba + ((__cluster - 2) * bs.sectors_per_cluster))

typedef struct _FAT_CONTEXT {
    /* Common LBAs */
    ULONG  fat_lba;
    ULONG  root_lba;
    ULONG  data_lba;

    UINT16 *fat_sector;
    UINT8  *root_sector;
    UINT8  *data_sector;
} FAT_CONTEXT;

/* Define FAT16 BIOS parameter block */
typedef struct _FAT16_BS {
  UINT8  bootjmp[3];
	UINT8  oem_name[8];
	UINT16 bytes_per_sector;
	UINT8	 sectors_per_cluster;
	UINT16 reserved_sector_count;
	UINT8  table_count;
	UINT16 root_entry_count;
	UINT16 total_sectors_16;
	UINT8	 media_type;
	UINT16 table_size_16;
	UINT16 sectors_per_track;
	UINT16 head_side_count;
	ULONG  hidden_sector_count;
	ULONG  total_sectors_32;
  UINT8	 bios_drive_num;
	UINT8	 reserved1;
	UINT8	 boot_signature;
	ULONG  volume_id;
	UINT8	 volume_label[11];
	UINT8	 fat_type_label[8];
} __attribute__((packed)) FAT16_BS;

typedef struct _DIR_ENTRY {
    UINT8  name[8];
    UINT8  ext[3];
    UINT8  attr;
    UINT8  reserved;
    UINT8  time[3];
    UINT16 crt_date;
    UINT16 acc_date;
    UINT16 high_cluster;
    UINT8  written_date[4];
    UINT16 low_cluster;
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
FatOpen(CHAR *path, ULONG *outsize);