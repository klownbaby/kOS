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

static free_chunk_t *kfree_list;

/* Little debug helper for dumping the current free list */
static void
dump_freelist()
{
    free_chunk_t *current = kfree_list;

    printk("----- CHUNK DUMP -----\n");

    do {
        // print our chunk
        printk("Chunk %x, size %x\n", current, current->size);

        // iterate
        current = current->next;
    } while (current != kfree_list);
}

/* Remove the chunk we are allocating from the free list */
static bool
remove_from_freelist(free_chunk_t *chunk)
{
    bool removed = FALSE;
    free_chunk_t *current = kfree_list;

    do {
        // is this our chunk?
        if (current == chunk)
        {
            // if we're the head of the free list, reset it
            if (current == kfree_list)
            {
                kfree_list = current->next;
            }

            // remove from linked list
            current->next->prev = current->prev;
            current->prev->next = current->next;

            // we're done
            removed = TRUE;
            break;
        }

        // iterate
        current = current->next;
    } while (current != kfree_list);

fail:
    return removed;
}

/* Split a larger chunk into a smaller one and adjust */
static void
split_chunk(free_chunk_t *to_alloc_chunk, size_t size)
{
    free_chunk_t *truncated_chunk = NULL;

    // manually increment new chunk pointer by calc'd offset
    truncated_chunk =
        (free_chunk_t *)((uint32_t)to_alloc_chunk + (size + sizeof(free_chunk_t)));

    // decrease existing size accordingly
    truncated_chunk->next = to_alloc_chunk->next;
    truncated_chunk->size = to_alloc_chunk->size - size;
    truncated_chunk->prev = to_alloc_chunk;

    // chunk is now our smaller fragment
    to_alloc_chunk->size = size;
    to_alloc_chunk->next = truncated_chunk;

    // if we're the only one in the list, create circular link
    if (to_alloc_chunk->prev == to_alloc_chunk)
    {
        to_alloc_chunk->prev = truncated_chunk;
    }
}

/* Increment kernel heap aligned on page, map new pages */
void
ksbrk(size_t size)
{
    uint32_t map_start = 0;

    // size must be aligned on page boundary
    if (!IS_PAGE_ALIGNED(size))
    {
        // align our size up on page boundary
        size = PAGE_ALIGN_UP(size);
    }

    // start our mapping at the current tail of heap
    map_start = (uint32_t)(g_heap_end - g_heap_start);

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

    // map kernel heap at start vaddr
    ksbrk(KERNEL_HEAP_DEFAULT_SIZE);

    // set up our genesis chunk
    kfree_list = (free_chunk_t *)g_heap_start;

    // explicitly set size, and next to NULL
    kfree_list->size = KERNEL_HEAP_DEFAULT_SIZE - sizeof(free_chunk_t);
    kfree_list->next = kfree_list;
    kfree_list->prev = kfree_list;

    // we're done
    BOOT_LOG("Kernel heap initialized.");
}

/* Kernel heap allocator, very inefficient */
void* 
kmalloc(size_t size)
{
    void *alloc = NULL;
    bool found_chunk = FALSE;
    free_chunk_t *free_chunk = kfree_list;

    do {
        // is this the perfect chunk??
        if (free_chunk->size == size)
        {
            found_chunk = TRUE;
            break;
        }

        // check that our allocation fits
        if (free_chunk->size >= size)
        {
            found_chunk = TRUE;   
        }

        // if not, move on
        free_chunk = free_chunk->next;
    } while (free_chunk != kfree_list);

    // check that a suitable chunk was found
    KASSERT_GOTO_FAIL_MSG(
        !found_chunk,
        "Chunk of given size not available!\n");

    // if our chunk size matches exactly, we are done
    KASSERT_GOTO_SUCCESS(free_chunk->size == size);

    // since our chunk is bigger than the alloc, split
    split_chunk(free_chunk, size);

success:
    // remove chunk from freelist
    KASSERT_GOTO_FAIL_MSG(
      !remove_from_freelist(free_chunk),
      "Failed to remove chunk from freelist\n");

    // start of our alloc is just after chunk metadata
    alloc = (void *)((uint32_t)free_chunk + sizeof(free_chunk_t));

fail:
    return alloc;
}

void
kfree(void* addr)
{
    free_chunk_t *chunk = NULL;

    // get our current chunk's header
    chunk = (free_chunk_t *)((uint32_t)addr - sizeof(free_chunk_t));

    // insert at head of free list
    chunk->next = kfree_list;
    chunk->prev = kfree_list->prev;

    kfree_list->prev->next = chunk;
    kfree_list->prev = chunk;
    kfree_list = chunk;
}
