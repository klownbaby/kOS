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

#pragma once

#include <stddef.h>

/* Define our default kernel heap size */
#define KERNEL_HEAP_DEFAULT_SIZE 0x4000

/* Define type for heap free list */
typedef struct free_list {
    // next free buffer in linked list
    struct freelist* next;
} free_list_t;

/* Kernel heap API definition */
void
ksbrk(size_t size);

void 
kmalloc_init();

void*
kmalloc(size_t size);
