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

/* Increment kernel heap aligned on page, map new pages */
void
ksbrk(size_t size)
{
    uint32_t map_start = 0;

    // align our size up on page boundary
    size = PAGE_ALIGN_UP(size);
    // start our mapping at the current tail of heap
    map_start = (uint32_t)(g_heap_end - g_kernel_start);

    for (uint32_t i = map_start; i < size; i += PAGE_SIZE)
    {
        // map a new page
        pmm_map_page(pmm_alloc_next(), (uint32_t)g_heap_start + i);
    }

    // increment our heap end pointer
    g_heap_end += size;
}

/* Initialize kernel heap */
void 
kmalloc_init() 
{
    // set heap size to zero
    g_heap_end = g_heap_start;

    // zero out our kernel free list on init
    kmemset(&kfree_list, 0, sizeof(kfree_list));

    // map kernel heap at start vaddr
    ksbrk(KERNEL_HEAP_DEFAULT_SIZE);

    // we're done
    BOOT_LOG("Kernel heap initialized.");
}

/* Kernel heap allocator */
void* 
kmalloc(size_t size)
{
    return (void *)0x0;
}
