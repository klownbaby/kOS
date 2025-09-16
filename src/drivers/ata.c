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
#include "drivers/ata.h"

/* Select a drive to read/write */
static VOID 
selectDrive(UINT8 bus, UINT8 dn)
{
    switch (bus)
    {
        // master bus
        case ATA_MASTER:
            if (dn == ATA_MASTER) __outb(DRIVE_SEL, MASTER_DRIVE);
            else __outb(DRIVE_SEL, SLAVE_DRIVE);
            break;
        // slave bus
        case ATA_SLAVE:
            if (dn == ATA_MASTER) __outb(ATA_SLAVE_BASE + 6, SLAVE_DRIVE);
            else __outb(ATA_SLAVE_BASE + 6, SLAVE_DRIVE);
            break;
        default:
            KPanic("Invalid bus!");
            break;
    }
}

/* Wait 400 nano seconds for read delay */
static VOID 
delay400Ns(VOID)
{
    // ah fuck it, this will do for now
    for (ULONG i = 0; i < 4; i++);
}

/* Get drive status */
DRIVE_STATUS 
AtaDriveStatus(UINT8 drive)
{
    UINT8 status = __inb(STATUS);

    // check if drive is busy first
    if (status & (1 << BSY)) return BSY;
    if (status & (1 << ERR)) return ERR;
    if (status & (1 << RDY)) return RDY;

    return status & (1 >> DF);
}

/* Read n sectors from disk into outData */
VOID 
AtaReadSectors(
    UINT8 drive,
    ULONG sectorCount,
    ULONG lba,
    VOID *outData
)
{
    UINT16* tmp = NULL;

    // this is messy, but leaving for now
    // select our drive, lba, and sector count
    __outb(DRIVE_SEL, drive | (UINT8)((lba >> 24) & 0xF));
    __outb(FEATURES, 0x0);
    __outb(SECT_CNT, sectorCount);
    __outb(SECT_NUM, (UINT8) lba);
    __outb(CYL_LOW, (UINT8) lba >> 8);
    __outb(CYL_HIGH, (UINT8) lba >> 16);
    __outb(COMMAND, READ_SECTORS);

    // dereference our out pointer as word pointer
    tmp = (UINT16 *)outData;

    // pretty sure 256 byte sectors
    for(ULONG i = 0; i < (sectorCount * 256); i += 256)
    {
        // poll drive status
        while(AtaDriveStatus(0) == BSY);

        for(ULONG j = 0; j < 256; ++j) 
        {
            // copy one word at a time
            tmp[j + i] = __inw(ATA_BASE);
        }

        // ensure we delay
        delay400Ns();
    }

    // set our out pointer
    outData = (VOID *)tmp;
}
