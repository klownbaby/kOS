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

/* Define FAT16 BIOS parameter block */
typedef struct fat16_bpb {
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
} __attribute__((packed)) fat16_bpb_t;

void
fat_dump_bpb();
