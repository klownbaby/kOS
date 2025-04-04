#pragma once

#include <stdint.h>

/* Boolean types */
#define TRUE  1
#define FALSE 0

/* Define function pointer types */
typedef void (*keyboard_notify_cb)(uint8_t scan, uint8_t pressed);

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

/* Keypress scan code enum for readability */
typedef enum keypress {
    KEY_BACKSPACE = 14,
    KEY_ENTER = 28,
    KEY_TILDE = 41,
    KEY_SHIFT = 42,
    KEY_CAPS = 54,
    KEY_UP_ARROW = 96,
} keypress_t;

/* Epoch date type */
typedef union epoch_date {
    uint16_t raw;

    struct {
        uint8_t day : 4;
        uint8_t month : 4;
        uint8_t year;
    } fields;
} epoch_date_t;

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

typedef struct write_callback {
    /* Current callback */
    void (*cb)(void* data);
    /* Next callback in list */
    struct write_callback* next;
} write_callback_t;

typedef struct pipe {
    /* Write notification callback */
    write_callback_t* write_cb_list;
    /* Pointer to data buffer */
    void* buf;
} pipe_t;

typedef struct file {
    /* Filename on disk */
    char* name;
    /* Size in clusters */
    uint32_t size;

    /* Standard file ops */
    void (*read)(void* outbuf);
    void (*write)(void* inbuf);
    void (*delete)(void);
} file_t;
