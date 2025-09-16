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

/* Define CMOS and RTC constants */
#define CMOS_BASE 0x70
#define CMOS_DATA 0x71

#define SECOND  0x00
#define MINUTE  0x02
#define HOUR    0x04
#define WEEKDAY 0x06
#define DAY     0x07
#define MONTH   0x08
#define YEAR    0x09

/* RTC function definitions */
VOID 
RtcInit(VOID);

VOID 
RtcCallback(VOID);
