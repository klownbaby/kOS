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

/* Initialize empty free list */
static FREE_CHUNK *kFreeList;

/* Remove the chunk we are allocating from the free list */
static VOID
removeFromFreeList(FREE_CHUNK *chunk)
{
    if (chunk == kFreeList)
    {
        kFreeList = kFreeList->next;
    }

    chunk->next->prev = chunk->prev;
    chunk->prev->next = chunk->next;
}

static VOID
addToFreeList(FREE_CHUNK *chunk)
{
    chunk->prev = kFreeList->prev;

    kFreeList->prev->next = chunk;
    kFreeList->prev = chunk;

    chunk->next = kFreeList;
    kFreeList = chunk;
}

/* Split a larger chunk into a smaller one and adjust */
static VOID
splitChunk(FREE_CHUNK *toSplit, SIZE size)
{
    FREE_CHUNK *new_chunk = NULL;

    // manually increment new chunk pointer by calc'd offset
    new_chunk =
        (FREE_CHUNK *)((ULONG)toSplit + (size + sizeof(FREE_CHUNK)));

    new_chunk->size = toSplit->size - (size + sizeof(FREE_CHUNK));

    // reset size
    toSplit->size = size;

    addToFreeList(new_chunk);
}


/* Little debug helper for dumping the current free list */
VOID
DumpFreeList(VOID)
{
    FREE_CHUNK *current = kFreeList;

    KPrint("----- CHUNK DUMP -----\n");

    do {
        // print our chunk
        KPrint("Chunk 0x%x, size 0x%x\n", current, current->size);

        // iterate
        current = current->next;
    } while (current != kFreeList);
}

/* Increment kernel heap aligned on page, map new pages */
VOID
KSbrk(SIZE size)
{
    ULONG mapStart = 0;

    // size must be aligned on page boundary
    if (!IS_PAGE_ALIGNED(size))
    {
        // align our size up on page boundary
        size = PAGE_ALIGN_UP(size);
    }

    // start our mapping at the current tail of heap
    mapStart = (ULONG)(g_HeapEnd - g_HeapStart);

    for (ULONG i = mapStart; i < size; i += PAGE_SIZE)
    {
        // map a new page
        PmmMapPage(
            NULL,
            PmmAllocNext(),
            (ULONG)g_HeapStart + i);
    }

    // increment our heap end pointer
    g_HeapEnd += size;
}

/* Initialize kernel heap */
VOID 
KMallocInit(VOID)
{
    // set heap size to zero
    g_HeapEnd = g_HeapStart;

    // map kernel heap at start vaddr
    KSbrk(KERNEL_HEAP_DEFAULT_SIZE);

    // set up our genesis chunk
    kFreeList = (FREE_CHUNK *)g_HeapStart;

    // explicitly set size, and next to NULL
    kFreeList->size = KERNEL_HEAP_DEFAULT_SIZE - sizeof(FREE_CHUNK);
    kFreeList->next = kFreeList;
    kFreeList->prev = kFreeList;

    // we're done
    BOOT_LOG("Kernel heap initialized.");
}

/* Kernel heap allocator, very inefficient */
VOID* 
KMalloc(SIZE size)
{
    VOID *alloc = NULL;
    FREE_CHUNK *foundChunk = NULL;
    FREE_CHUNK *freeChunk = kFreeList;

    do {
        // perfect fit?
        if (freeChunk->size == size)
        {
            foundChunk = freeChunk;
            goto success;
        }

        // does this chunk fit?
        if (freeChunk->size >= size)
        {
            // if we haven't found a chunk yet, take this one
            if (!foundChunk)
            {
                foundChunk = freeChunk;
            }
            // is this one smaller than the previously found?
            else if (foundChunk && freeChunk->size < foundChunk->size)
            {
                // if so, update to a better fit
                foundChunk = freeChunk;
            }
        }

        // if not, move on
        freeChunk = freeChunk->next;
    } while (freeChunk != kFreeList);

    // check that a suitable chunk was found
    KASSERT_GOTO_FAIL_MSG(
        foundChunk == NULL,
        "Chunk of given size not available!\n");

    if (foundChunk->size >= 0x30)
    {
        // since our chunk is bigger than the alloc, split
        splitChunk(foundChunk, size);
    }

success:
    // remove chunk from freelist
    removeFromFreeList(foundChunk);

    // start of our alloc is just after chunk metadata
    alloc = (VOID *)((ULONG)foundChunk + sizeof(FREE_CHUNK));

    // by default, zero each newly allocated buffer
    KMemSet(alloc, 0, size);

fail:
    return alloc;
}

VOID
KFree(VOID* addr)
{
    FREE_CHUNK *chunk = NULL;

    // get our current chunk's header
    chunk = (FREE_CHUNK *)((ULONG)addr - sizeof(FREE_CHUNK));

    KASSERT_PANIC(
      (ULONG)chunk < g_HeapStart || (ULONG)chunk >= g_HeapEnd,
      "Chunk is corrupted!");

    // add new chunk to free list
    addToFreeList(chunk);
}

MODULE_ENTRY_ORDERED(KMallocInit, 4);
