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
#include "drivers/tty.h"
#include "drivers/fat.h"

/* Handle clear ksh command (clear screen) */
void
handle_clear(uint32_t argc, char **argv)
{
    tty_clear();
}

/* Handle reboot ksh command (warm reboot) */
void
handle_reboot(uint32_t argc, char **argv)
{
    warm_reboot();
}

/* Handle dumpt ksh command (dump page tables) */
void
handle_dumpt(uint32_t argc, char **argv)
{
    pmm_dumpt();
}

/* Handle dumpfs ksh command (dump FAT BIOS parameter block) */
void
handle_dumpfs(uint32_t argc, char **argv)
{
    fat_dump_bs();
}

/* Handle neofetch ksh command (dumb lol) */
void
handle_neofetch(uint32_t argc, char **argv)
{
    tty_neofetch();
}
