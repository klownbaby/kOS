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
#include "drivers/vga.h"

/* Initialize default VGA color */
static VGA_COLOR vgaColor;

/* For now, just clear the screen on init (also sets default color) */
VOID 
VgaInit(VOID) 
{
    // just clear the screen for now
    VgaClear();
}

/* Sets color entry for VGA buffer */
VOID 
VgaSetColor(VGA_COLOR fg, VGA_COLOR bg) 
{
    vgaColor = VGA_ENTRY_COLOR(fg, bg);
}

/* Puts a CHARacter into the VGA buffer at row, col */
VOID 
VgaPutC(CHAR c, SIZE row, SIZE col) 
{
    UINT16* vgaBuffer = (UINT16 *)VGA_BASE;
    const SIZE vga_index = row * VGA_WIDTH + col;

    vgaBuffer[vga_index] = VGA_ENTRY(c, vgaColor);
}

/* Shift VGA buffer one column down */
VOID 
VgaScroll(VOID)
{
    UINT16 *vgaBuffer = (UINT16 *)VGA_BASE;
    SIZE index = 0;
    SIZE scrollIndex = 0;
    UINT16 entry = 0;

    // loop through every row and col (80 * 25)
    for (int row = 1; row < VGA_HEIGHT; row++) 
    {
        for (int col = 0; col < VGA_WIDTH; col++) 
        {
            // get current index of vga entry
            index = row * VGA_WIDTH + col;
            // move index up one row and retain column
            scrollIndex = (row - 1) * VGA_WIDTH + col;

            // set newe buffer entry
            entry = vgaBuffer[index];

            // clear previous index and set new
            vgaBuffer[index] = VGA_ENTRY(' ', vgaColor);
            vgaBuffer[scrollIndex] = entry;
        }
    }
}

VOID 
VgaSetBar(VGA_COLOR fg, VGA_COLOR bg, const CHAR* str, SIZE offset)
{
    UINT16* vgaBuffer = (UINT16 *)VGA_BASE;
    UINT16 color = VGA_ENTRY_COLOR(fg, bg);

    SIZE len = KStrLen(str);

    for (SIZE i = 0; i < VGA_WIDTH; ++i) 
    {
        if (i >= offset && i < (offset + len)) 
        {
            vgaBuffer[24 * VGA_WIDTH + i] = VGA_ENTRY(str[i - offset], color);
        } else {
            vgaBuffer[24 * VGA_WIDTH + i] = VGA_ENTRY(' ', color);
        }
    }
}

/* Clear VGA buffer */
VOID 
VgaClear(VOID) 
{
    UINT16* vgaBuffer = (UINT16 *)VGA_BASE;
    UINT16 vga_index = 0;

    // wipe out our vga buffer
    for (SIZE i = 0; i < VGA_HEIGHT; ++i) 
    {
        for (SIZE j = 0; j < VGA_WIDTH; ++j) 
        {
            // get our next index to reset
            vga_index = i * VGA_WIDTH + j;

            // reset it
            vgaBuffer[vga_index] = VGA_ENTRY(' ', vgaColor);
        }
    }
}

/* Disalbes cursor */
VOID 
VgaCursorDisable(VOID) 
{
    // disable cursor by writing to 0x3d4 and 0x3d5
    __outb(0x3D4, 0x0A);
    __outb(0x3D5, 0x20);
}

/* Enables cursor */
VOID 
VgaCursorEnable(UINT8 start, UINT8 end) 
{
    // set starting scanline of cursor
    __outb(0x3D4, 0x0A);
    __outb(0x3D5, (__inb(0x3D5) & 0xC0) | start);

    // set ending scanline of cursor (for cursor shapes)
    __outb(0x3D4, 0x0B);
    __outb(0x3D5, (__inb(0x3D5) & 0xE0) | end);
}

/* Update cursor to new x, y pos (row, col for tty) */
VOID 
VgaUpdateCursor(int x, int y)
{
	UINT16 pos = y * VGA_WIDTH + x;
 
    __outb(0x3D4, 0x0F);
    __outb(0x3D5, (UINT8) (pos & 0xFF));
    __outb(0x3D4, 0x0E);
    __outb(0x3D5, (UINT8) ((pos >> 8) & 0xFF));
}

/* Gets cursor position and returns struct of CURSOR_POS (x, y) */
CURSOR_POS 
VgaGetCursorPosition(VOID) 
{
    UINT16 pos = 0;
    CURSOR_POS cursor_pos;

    __outb(0x3D4, 0x0F);
    pos |= __inb(0x3D5);

    __outb(0x3D4, 0x0E);
    pos |= ((UINT16)__inb(0x3D5)) << 8;

    cursor_pos.x = pos / VGA_WIDTH; 
    cursor_pos.y = pos % VGA_WIDTH; 

    return cursor_pos;
}

