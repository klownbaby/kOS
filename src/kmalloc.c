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
#include "pmm.h"

/* Initialize our kernel free list */
static free_list_t kfree_list;

#define TEST ((uint32_t*)0xd02000)

void 
kmalloc_init() 
{
    size_t heap_size = 0;
    uint32_t test = 0xc0a00000;

    pmm_map_page(test, (uint32_t)TEST);
    TEST[0] = 1;

    // zero out our kernel free list on init
    kmemset(&kfree_list, 0, sizeof(kfree_list));

    // we're donw
    BOOT_LOG("Kernel heap initialized.");
}

void* 
kmalloc(size_t size)
{
    return (void *)0x0;
}
