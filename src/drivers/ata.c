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


/* Get drive status */
drive_status_t 
drive_status(uint8_t drive)
{
    uint8_t status = inb(STATUS);

    // check if drive is busy first
    if (status & (1 << BSY)) return BSY;
    if (status & (1 << ERR)) return ERR;
    if (status & (1 << RDY)) return RDY;

    return status & (1 >> DF);
}

/* Select a drive to read/write */
void 
select_drive(uint8_t bus, uint8_t dn)
{
    switch (bus)
    {
        // master bus
        case ATA_MASTER:
            if (dn == ATA_MASTER) outb(DRIVE_SEL, MASTER_DRIVE);
            else outb(DRIVE_SEL, SLAVE_DRIVE);
            break;
        // slave bus
        case ATA_SLAVE:
            if (dn == ATA_MASTER) outb(ATA_SLAVE_BASE + 6, SLAVE_DRIVE);
            else outb(ATA_SLAVE_BASE + 6, SLAVE_DRIVE);
            break;
        default:
            kpanic("Invalid bus!");
            break;
    }
}

/* Wait 400 nano seconds for read delay */
void 
delay_400ns()
{
    for (int i = 0; i < 4; i++);
}

/* Read n sectors from disk */
void 
rw_sectors(
    uint8_t mode,
    uint8_t drive,
    uint32_t sector_count,
    uint32_t lba,
    void* dest
)
{
    outb(DRIVE_SEL, drive | (uint8_t) ((lba >> 24) & 0xF));
    outb(FEATURES, 0x0);
    outb(SECT_CNT, sector_count);
    outb(SECT_NUM, (uint8_t) lba);
    outb(CYL_LOW, (uint8_t) lba >> 8);
    outb(CYL_HIGH, (uint8_t) lba >> 16);
    outb(COMMAND, READ_SECTORS);

    uint16_t* tmp = (uint16_t*) dest;

    for(int i = 0; i < (sector_count * 256); i += 256)
    {
        // poll drive status
        while(drive_status(0) == BSY);

        for(int j = 0; j < 256; ++j) {
            tmp[j + i] = inw(ATA_BASE);
        }

        delay_400ns();
    }

    dest = (void*)tmp;
}
