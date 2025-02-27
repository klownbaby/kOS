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

/* Initialize our kernel free list */
static free_list_t kfree_list;

void 
kmalloc_init() 
{
    uint32_t low = PAGE_ALIGN_DOWN(g_kernel_start);
    uint32_t high = PAGE_ALIGN_UP(g_kernel_end);

    // zero out our kernel free list on init
    kmemset(&kfree_list, 0, sizeof(kfree_list));

    // map each kernel page as used
    for (uint32_t frame = low; frame < high; frame += PAGE_SIZE)
    {
        // allocate our page in pmm_bitmap
        // KASSERT_PANIC(
        //     pmm_alloc_frame(frame) != STATUS_SUCCESS, 
        //     "Kernel heap initialization failed!\n");
    }

    // we're donw
    BOOT_LOG("Kernel heap initialized.");
}

void* 
kmalloc(size_t size)
{
    return (void *)0x0;
}
