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

static free_chunk_t* kfree_list;

/* Split a larger chunk into a smaller one and adjust */
static void
split_chunk(free_chunk_t* chunk, size_t size)
{
    free_chunk_t* new_chunk = NULL;
    free_chunk_t* prev_chunk = NULL;

    // manually increment new chunk pointer by calc'd offset
    new_chunk = (free_chunk_t*)((uint32_t)chunk + (size + sizeof(free_chunk_t)));

    // decrease existing size accordingly
    new_chunk->next = chunk->next;
    new_chunk->size = chunk->size - size;

    // store previous chunk temporarily
    prev_chunk = kfree_list;

    // chunk is now our smaller fragment
    chunk->size = size;
    chunk->next = new_chunk;
    chunk->prev = prev_chunk;

    // finally, add new chunk to head of free list
    kfree_list = new_chunk;
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
    kfree_list = (free_chunk_t*)g_heap_start;

    // explicitly set size, and next to NULL
    kfree_list->size = KERNEL_HEAP_DEFAULT_SIZE;
    kfree_list->next = NULL;
    kfree_list->prev = NULL;

    // we're done
    BOOT_LOG("Kernel heap initialized.");
}

/* Kernel heap allocator, very inefficient */
void* 
kmalloc(size_t size)
{
    void* alloc = NULL;
    free_chunk_t* allocated_chunk = NULL;
    free_chunk_t* free_chunk = kfree_list;

    while (free_chunk != NULL)
    {
        // check that our allocation fits
        if (free_chunk->size >= size) break;

        // if not, move on
        free_chunk = free_chunk->next;
    }

    // check that a suitable chunk was found
    KASSERT_GOTO_FAIL_MSG(
        free_chunk == NULL, 
        "Chunk of given size not available!\n");

    // start of our alloc is just after chunk metadata
    alloc = (void*)((uint32_t)free_chunk + sizeof(free_chunk_t));

    // if our chunk size matches exactly, we are done
    KASSERT_GOTO_SUCCESS(free_chunk->size == size);

    // since our chunk is bigger than the alloc, split
    split_chunk(free_chunk, size);

fail:
success:
    return alloc;
}

void
kfree(void* addr)
{
    free_chunk_t* header = NULL;

    // get our current chunk's header
    header = (free_chunk_t*)(addr - sizeof(free_chunk_t));

    // insert at head of free list
    header->next = kfree_list;
    kfree_list = header;
}
