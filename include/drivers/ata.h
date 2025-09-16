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

/* Define ATA bases */
#define ATA_BASE           0x1F0
#define ATA_SLAVE_BASE     0x170
#define ATA_MASTER          0x00
#define ATA_SLAVE           0x01

/* Define ATA offsets */
#define DATA        ATA_BASE + 0
#define ERREG       ATA_BASE + 1
#define FEATURES    ATA_BASE + 1
#define SECT_CNT    ATA_BASE + 2
#define SECT_NUM    ATA_BASE + 3
#define CYL_LOW     ATA_BASE + 4
#define CYL_HIGH    ATA_BASE + 5
#define DRIVE_SEL   ATA_BASE + 6
#define STATUS      ATA_BASE + 7
#define COMMAND     ATA_BASE + 7

/* Define drive constants */
#define MASTER_DRIVE        0xE0
#define SLAVE_DRIVE         0xF0
#define IDENTIFY            0xEC
#define READ_SECTORS        0x20
#define WRITE_SECTORS       0x30

/* ATA drive status */
typedef enum _DRIVE_STATUS {
    ERR,
    IDX,
    CORR,
    DRQ,
    SRV,
    DF,
    RDY,
    BSY
} DRIVE_STATUS;

/* ATA function definitions */
DRIVE_STATUS 
AtaDriveStatus(UINT8 drive);

VOID 
AtaReadSectors(UINT8 drive, ULONG sector_count, ULONG lba, VOID* outdata);
