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

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

/* Boolean types */
#define TRUE  1
#define FALSE 0

/* Define maximum file descriptor index */
#define MAX_FILE_DESCRIPTORS 3

/* Define a standard status check return value */
typedef enum _KSTATUS {
    STATUS_SUCCESS,
    STATUS_FAILED,
    STATUS_INVALID,
    STATUS_NOT_CONFIGURED,
    STATUS_INSUFFICIENT_SPACE,
    STATUS_IN_USE,
    STATUS_UNKNOWN
} KSTATUS;

/* Alias common types */
typedef void     VOID;
typedef int32_t  LONG;
typedef uint32_t ULONG;
typedef uint16_t UINT16;
typedef uint8_t  UINT8;
typedef uint8_t  BOOLEAN;
typedef size_t   SIZE;
typedef char     CHAR;
typedef unsigned char UCHAR;
typedef multiboot_info_t MULTIBOOT_INFO;

/* Define function pointer types */
typedef VOID    (*MODULE_ENTRY)(VOID);
typedef LONG    (*PROC_ENTRY)(VOID);
typedef VOID    (*KEYBOARD_NOTIFY)(UINT8 scan, UINT8 pressed);
typedef KSTATUS (*FILE_READ)(VOID *outBuffer, SIZE size);
typedef KSTATUS (*FILE_WRITE)(VOID *inBuffer, SIZE size);

/* Define a temporary file type */
typedef struct _FILE {
    FILE_READ read;
    FILE_WRITE write;
} FILE;

/* Define file descriptor types, just STD for now */
typedef enum _FILE_DESCRIPTOR {
    STDIN,
    STDOUT,
    STDERR
} FILE_DESCRIPTOR;

/* Define process handle type (process cr3 phys) */
typedef struct _PROC_HANDLE {
    ULONG cr3;
    ULONG size;
    FILE fileBindings[MAX_FILE_DESCRIPTORS];
    PROC_ENTRY entry;
    VOID* buffer;
} PROC_HANDLE;

/* Hardware text mode color constants */
typedef enum _VGA_COLOR {
    VGA_COLOR_BLACK,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_LIGHT_BROWN,
    VGA_COLOR_WHITE
} VGA_COLOR;

/* Define global tty state context */
typedef struct _TTY_STATE {
    SIZE row;
    SIZE col;

    /* Foreground/background color entries */
    VGA_COLOR fgColor;
    VGA_COLOR bgColor;
} TTY_STATE;

/* Keypress scan code enum for readability */
typedef enum _KEYPRESS {
    KEY_BACKSPACE = 14,
    KEY_ENTER = 28,
    KEY_TILDE = 41,
    KEY_SHIFT = 42,
    KEY_CAPS = 54,
    KEY_UP_ARROW = 96,
} KEYPRESS;

/* Epoch date type */
typedef union _EPOCH_DATE {
    UINT16 raw;

    struct {
        UINT8 year : 7;
        UINT8 month : 4;
        UINT8 day : 5;
    } fields;
} EPOCH_DATE;

/* Physical memory bitmap entry */
typedef struct _PMM_BITMAP_ENTRY {
    /* Set single bitfield for efficiency */
    UINT8 used: 1;
} PMM_BITMAP_ENTRY;

/* Heap free list */
typedef struct _FREE_CHUNK {
    /* Size of free buffer */
    ULONG size;
    /* Next free buffer node */
    struct _FREE_CHUNK* next;
    /* Previous free buffer node */
    struct _FREE_CHUNK* prev;
} FREE_CHUNK;
