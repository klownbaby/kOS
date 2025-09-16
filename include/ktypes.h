#pragma once

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

/* Boolean types */
#define TRUE  1
#define FALSE 0

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
typedef VOID (*MODULE_ENTRY)(VOID);
typedef LONG (*PROC_ENTRY)(ULONG argc, CHAR **argv);
typedef VOID (*KEYBOARD_NOTIFY)(UINT8 scan, UINT8 pressed);

/* Define process handle type (process cr3 phys) */
typedef struct _PROC_HANDLE {
    ULONG cr3;
    ULONG size;
    PROC_ENTRY entry;
} PROC_HANDLE;

/* Define a temporary file type */
typedef struct _FILE {
    VOID* buffer;
    ULONG size;
} FILE;

/* Hardware text mode color constants */
typedef enum _VGA_COLOR {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} VGA_COLOR;


/* Define global tty state context */
typedef struct _TTY_STATE {
    SIZE      row;
    SIZE      col;

    VGA_COLOR fgColor;
    VGA_COLOR bgColor;
} TTY_STATE;

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
