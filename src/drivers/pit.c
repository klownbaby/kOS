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
#include "drivers/pit.h"
#include "drivers/tty.h"
#include "drivers/rtc.h"

/* Initialize count down for sleep function */
static volatile ULONG count_down;

/* Awful, TODO: change later */
VOID 
PitPoll(VOID)
{
    RtcCallback();
}

/* Decrement counter on clock edge */
VOID 
PitCallback(__attribute__((unused)) INTERRUPT_REGISTER_CONTEXT registers)
{
    --count_down;

    PitPoll();
}

/* Global kernel sleep function, sleep for x milliseconds */
VOID 
PitSleep(ULONG millis)
{
    count_down = millis;

    while (count_down > 0);
}

VOID 
PitInit(VOID)
{
    // set divisor for channel 0
    ULONG divisor = FREQUENCY/RELOAD;

    RegisterInterruptHandler(IRQ0, PitCallback);

    // select square wave generator
    __outb(PIT_COMMAND, 0x36);
    __outb(PIT_CHANNEL0, (UINT8) (divisor & 0xFF));
    __outb(PIT_CHANNEL0, (UINT8) ((divisor >> 8) & 0xFF));

    BOOT_LOG("PIT initialized.")
}

MODULE_ENTRY(PitInit);
