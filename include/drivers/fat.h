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
    (fat_ctx.data_lba + ((__cluster - 2) * bs.sectors_per_cluster))

typedef struct fat_context {
    /* Common LBAs */
    uint32_t fat_lba;
    uint32_t root_lba;
    uint32_t data_lba;

    uint16_t* fat_sector;
    uint8_t* root_sector;
    uint8_t* data_sector;
} fat_context_t;

/* Define FAT16 BIOS parameter block */
typedef struct fat16_bs {
  uint8_t  bootjmp[3];
	uint8_t  oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t	 sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t  table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t	 media_type;
	uint16_t table_size_16;
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;
  uint8_t	 bios_drive_num;
	uint8_t	 reserved1;
	uint8_t	 boot_signature;
	uint32_t volume_id;
	uint8_t	 volume_label[11];
	uint8_t	 fat_type_label[8];
} __attribute__((packed)) fat16_bs_t;

typedef struct dir_entry {
    uint8_t  name[8];
    uint8_t  ext[3];
    uint8_t  attr;
    uint8_t  reserved;
    uint8_t  time[3];
    uint16_t crt_date;
    uint16_t acc_date;
    uint16_t high_cluster;
    uint8_t  written_date[4];
    uint16_t low_cluster;
    uint32_t size;
} dir_entry_t;

void
fat_dump_bs();

void
fat_dump_dentry(dir_entry_t *dentry);

void
fat_dump_directory(void *buffer);

void
fat_dump_root();

void *
fat_open(char *path, uint32_t *outsize);

void
fat_init();
