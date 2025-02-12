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
static volatile uint32_t count_down;

/* Decrement counter on clock edge */
void 
pit_callback(__attribute__((unused)) i_register_t registers)
{
    --count_down;

    poll();
}

/* Global kernel sleep function, sleep for x milliseconds */
void 
sleep(uint32_t millis)
{
    count_down = millis;

    while (count_down > 0);
}

void 
pit_init()
{
    // set divisor for channel 0
    uint32_t divisor = FREQUENCY/RELOAD;

    register_interrupt_handler(IRQ0, pit_callback);

    // select square wave generator
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8_t) (divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t) ((divisor >> 8) & 0xFF));

    BOOT_LOG("PIT initialized.")
}

void 
poll()
{
    rtc_callback();
}
