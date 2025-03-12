#pragma once

#include <stdint.h>

/* Null typdef */
#define NULL (void *) 0x0

/* Define a standard status check return value */
typedef enum kstatus {
  STATUS_SUCCESS,
  STATUS_FAILED,
  STATUS_INVALID,
  STATUS_NOT_CONFIGURED,
  STATUS_INSUFFICIENT_SPACE,
  STATUS_IN_USE,
  STATUS_UNKNOWN
} kstatus_t;

/* Physical memory bitmap entry */
typedef struct pmm_bitmap_entry {
    /* Set single bitfield for efficiency */
    uint8_t used: 1;
} pmm_bitmap_entry_t;

/* Heap free list */
typedef struct free_chunk {
    /* Size of free buffer */
    uint32_t size;
    /* Next free buffer node */
    struct free_chunk* next;
} free_chunk_t;
