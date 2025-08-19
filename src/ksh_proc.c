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
#include "kutils.h"

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

/* Handle dumpfl ksh command (dump free list) */
void
handle_dumpfl(uint32_t argc, char **argv)
{
    dump_freelist();
}

/* Handle neofetch ksh command (dumb lol) */
void
handle_neofetch(uint32_t argc, char **argv)
{
    tty_neofetch();
}

/* Poke (read) from a given memory address */
void
handle_poke(uint32_t argc, char **argv)
{
    uint32_t addr = 0;
    uint32_t size = 0;
    uint8_t *buf = NULL;

    KASSERT_GOTO_FAIL_MSG(argc < 3, "Usage: poke [address] [size]\n");

    // get address and size from args
    addr = katoi(argv[1]);
    size = katoi(argv[2]);

    // for now, restrict size to 32 bytes
    KASSERT_GOTO_FAIL_MSG(size > 0x10, "Inavlid size, must be less than 32!\n");

    // allocate a scratch buffer
    buf = kmalloc(size);

    // copy desired size
    kmemcpy(buf, (void *)addr, size);

    // finally, dump
    printk("Dumping at address (0x%x)\n", addr);
    printk("    { ");

    for (uint32_t i = 0; i < size; ++i)
    {
        printk("0x%x, ", buf[i]);
    }

    printk("}\n");

fail:
    // ensure we free our scratch buffer
    if (buf) kfree(buf);

    return;
}

/* Prod (write) to a given memory address */
void
handle_prod(uint32_t argc, char **argv)
{
    
}

void
handle_cat(uint32_t argc, char **argv)
{
    char *data = NULL;
    uint32_t size = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: cat [file name]\n");

    data = fat_open(argv[1], &size);

    KASSERT_GOTO_FAIL_MSG(data == NULL, "File not found!\n");

    for (uint32_t i = 0; i < size; ++i)
    {
        printk("%c", data[i]);
    }

fail:
    if (data)
    {
        kfree(data);
    }

    return;
}
