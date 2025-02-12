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

bool 
cmos_update()
{
    outb(CMOS_BASE, 0x0A);

    return inb(CMOS_DATA) & 0x80;
}

unsigned char 
rtc_register(uint8_t rtc_reg)
{
    outb(CMOS_BASE, rtc_reg);

    return inb(CMOS_DATA);
}

void 
rtc_callback()
{
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char month;

    minute = rtc_register(MINUTE);
    day = rtc_register(DAY);
    hour = rtc_register(HOUR);
    month = rtc_register(MONTH);

    bool regb = rtc_register(0x0B) & 0x04;

    if (!regb) {
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
    }
}

void 
rtc_init()
{
    // register_interrupt_handler(IRQ8, rtc_callback);

    // outb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
    // outb(0x71, 0x20);	// write to CMOS/RTC RAM

    BOOT_LOG("RTC initialized.");
}
