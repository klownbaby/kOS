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
#include "drivers/vga.h"

/* Init tty interface and set default color to white on black */
VOID 
TTYInit(VOID)
{
    // set foreground color to white, background to blue
    // feel free to change this
    TTYSetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    // initialize vga interface
    VgaInit();

    // disable cursor until shell inits
    VgaCursorDisable();

    // check GRUB version
    TTYWriteColor("Booted with ", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    TTYWriteColor((const CHAR *)g_Mbd->boot_loader_name, VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    TTYWrite("\n\n");
}

/* Write a string to tty output, with row, col tracking */
VOID 
TTYWrite(const CHAR* str) 
{
    // loop through each CHARacter and putc to screen
    for (SIZE i = 0; i < KStrLen(str); ++i) 
    {
        // detect when a newline CHARacter is present
        TTYPutC(str[i]);
    }
}

VOID 
TTYWriteColor(const CHAR* str, VGA_COLOR fg, VGA_COLOR bg) 
{
    // set color to desired
    VgaSetColor(fg, bg);

    // loop through each CHARacter and putc to screen
    TTYWrite(str);

    // set color back to default tty state
    VgaSetColor(g_TTYState.fgColor, g_TTYState.bgColor);
}

VOID 
TTYPutC(CHAR c)
{
    if (c != '\n') VgaPutC(c, g_TTYState.row, g_TTYState.col);

    // if we are at the end of the line (80 columns), break line
    if (++g_TTYState.col == VGA_WIDTH || c == '\n') 
    {
        g_TTYState.col = 0;

        // same for rows
        if (++g_TTYState.row == VGA_HEIGHT) 
        {
            VgaScroll();
            --g_TTYState.row;
        } 
    }

    // update cursor after string is written to tty
    VgaUpdateCursor(g_TTYState.col, g_TTYState.row);
}

/* Write a CHARacter to the screen relative to current g_TTYState */
VOID 
TTYPutCRelative(CHAR c, int relx, int rely, bool cursor)
{
    g_TTYState.col += relx;
    g_TTYState.row += rely;

    if (c != '\n') VgaPutC(c, g_TTYState.row, g_TTYState.col);

    // update cursor after string is written to tty
    if (cursor) VgaUpdateCursor(g_TTYState.col, g_TTYState.row);
}

/* Clear screen and reset row, col pointers */
VOID 
TTYClear(VOID) 
{
    // clear vga buffer
    VgaClear();

    // reset column and row pointers
    g_TTYState.col = 0;
    g_TTYState.row = 0;
}

/* Set new color and store fg/bg colors in state */
VOID 
TTYSetColor(VGA_COLOR fg, VGA_COLOR bg) 
{
    VgaSetColor(fg, bg);

    g_TTYState.fgColor = fg;
    g_TTYState.bgColor = bg;
}

/* Just a somewhat unecessary boot success message */
VOID 
TTYNeofetch(VOID) 
{
    // temporarily set text color to green
    VgaSetColor(VGA_COLOR_GREEN, g_TTYState.bgColor);
    TTYWrite("\nYou're not that guy, pal..\n");

    // set text color back to default
    VgaSetColor(g_TTYState.fgColor, g_TTYState.bgColor);

    // some dope ascii art that I definitely didn't generate...
    TTYWrite("\n\n");
    TTYWrite(" /$$        /$$$$$$   /$$$$$$\n");
    TTYWrite("| $$       /$$__  $$ /$$__  $$\n");
    TTYWrite("| $$   /$$| $$  \\ $$| $$  \\__/\n");
    TTYWrite("| $$  /$$/| $$  | $$|  $$$$$$ \n");
    TTYWrite("| $$$$$$/ | $$  | $$ \\____  $$\n");
    TTYWrite("| $$_  $$ | $$  | $$ /$$  \\ $$\n");
    TTYWrite("| $$ \\  $$|  $$$$$$/|  $$$$$$/\n");
    TTYWrite("|__/  \\__/ \\______/  \\______/ \n");
    TTYWrite("\nv0.0.2\n");

    TTYWrite("\n\nWelcome to kOS!\n");

    // enable vertical cursor (max scanline 15)
    VgaCursorEnable(1, 15);
    
    // update cursor to current row, col
    VgaUpdateCursor(g_TTYState.col, g_TTYState.row);
}

MODULE_ENTRY_ORDERED(TTYInit, 0);
