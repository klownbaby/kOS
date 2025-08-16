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
#include "ktypes.h"
#include "kutils.h"
#include "pmm.h"

static free_chunk_t *kfree_list;

/* Little debug helper for dumping the current free list */
void
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
static void
remove_from_freelist(free_chunk_t *chunk)
{
    if (chunk == kfree_list)
    {
        kfree_list = kfree_list->next;
    }

    chunk->next->prev = chunk->prev;
    chunk->prev->next = chunk->next;
}

static void
add_to_freelist(free_chunk_t *chunk)
{
    chunk->prev = kfree_list->prev;

    kfree_list->prev->next = chunk;
    kfree_list->prev = chunk;

    chunk->next = kfree_list;
    kfree_list = chunk;
}

/* Split a larger chunk into a smaller one and adjust */
static void
split_chunk(free_chunk_t *to_split, size_t size)
{
    free_chunk_t *new_chunk = NULL;

    // manually increment new chunk pointer by calc'd offset
    new_chunk =
        (free_chunk_t *)((uint32_t)to_split + (size + sizeof(free_chunk_t)));

    new_chunk->size = to_split->size - (size + sizeof(free_chunk_t));

    // reset size
    to_split->size = size;

    add_to_freelist(new_chunk);
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
    free_chunk_t *found_chunk = NULL;
    free_chunk_t *free_chunk = kfree_list;

    do {
        // does this chunk fit?
        if (free_chunk->size >= size)
        {
            // if we haven't found a chunk yet, take this one
            if (!found_chunk)
            {
                found_chunk = free_chunk;
            }
            // is this one smaller than the previously found?
            else if (found_chunk && free_chunk->size < found_chunk->size)
            {
                // if so, update to a better fit
                found_chunk = free_chunk;
            }
        }

        // if not, move on
        free_chunk = free_chunk->next;
    } while (free_chunk != kfree_list);

    // check that a suitable chunk was found
    KASSERT_GOTO_FAIL_MSG(
        found_chunk == NULL,
        "Chunk of given size not available!\n");

    // perfect fit?
    KASSERT_GOTO_SUCCESS(found_chunk->size == size);

    if (found_chunk->size >= 0x30)
    {
        // since our chunk is bigger than the alloc, split
        split_chunk(found_chunk, size);
    }

success:
    // remove chunk from freelist
    remove_from_freelist(found_chunk);

    // start of our alloc is just after chunk metadata
    alloc = (void *)((uint32_t)found_chunk + sizeof(free_chunk_t));

fail:
    return alloc;
}

void
kfree(void* addr)
{
    free_chunk_t *chunk = NULL;

    // get our current chunk's header
    chunk = (free_chunk_t *)((uint32_t)addr - sizeof(free_chunk_t));

    KASSERT_PANIC(
      (uint32_t)chunk < g_heap_start || (uint32_t)chunk >= g_heap_end,
      "Chunk is corrupted!");

    add_to_freelist(chunk);
}
