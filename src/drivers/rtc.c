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
#include "drivers/rtc.h"
#include "drivers/tty.h"

BOOLEAN 
CmosUpdate()
{
    __outb(CMOS_BASE, 0x0A);

    return __inb(CMOS_DATA) & 0x80;
}

UCHAR 
RtcRegister(UINT8 rtc_reg)
{
    __outb(CMOS_BASE, rtc_reg);

    return __inb(CMOS_DATA);
}

VOID 
RtcCallback()
{
    UCHAR day;
    UCHAR hour;
    UCHAR minute;
    UCHAR month;
    BOOLEAN regb = RtcRegister(0x0B) & 0x04;

    minute = RtcRegister(MINUTE);
    day = RtcRegister(DAY);
    hour = RtcRegister(HOUR);
    month = RtcRegister(MONTH);

    if (!regb) 
    {
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
    }
}

VOID 
RtcInit()
{
    // RegisterInterruptHandler(IRQ8, RtcCallback);

    // __outb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
    // __outb(0x71, 0x20);	// write to CMOS/RTC RAM

    BOOT_LOG("RTC initialized.");
}

MODULE_ENTRY(RtcInit);
