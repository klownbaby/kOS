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

/* Define vga dimensions and base address */
#define VGA_BASE 0xB8000
#define VGA_HEIGHT 25
#define VGA_WIDTH 80

/* VGA character entry */
#define VGA_ENTRY(c, color) \
	((UINT16) c | (UINT16) color << 8)

/* VGA foreground/background entry */
#define VGA_ENTRY_COLOR(fg, bg) \
	(fg | bg << 4)

typedef struct _CURSOR_POS {
    UINT8 x;
    UINT8 y;
} CURSOR_POS;
 
VOID 
VgaInit(VOID);

VOID
VgaClear(VOID);

VOID 
VgaScroll(VOID);

VOID 
VgaCursorDisable(VOID);

CURSOR_POS 
VgaGetCursorPosition(VOID);

VOID 
VgaSetColor(VGA_COLOR fg, VGA_COLOR bg);

VOID 
VgaPutC(CHAR c, SIZE row, SIZE col);

VOID 
VgaSetBar(VGA_COLOR fg, VGA_COLOR bg, const CHAR *str, SIZE offset);

VOID 
VgaCursorEnable(UINT8 start, UINT8 end);

VOID 
VgaUpdateCursor(int x, int y);

