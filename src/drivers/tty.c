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

static tty_state_t tty_state;

/* Init tty interface and set default color to white on black */
void 
tty_init() 
{
    // set foreground color to white, background to blue
    // feel free to change this
    tty_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    // initialize vga interface
    vga_init();

    vga_cursor_disable();
}

/* Write a string to tty output, with row, col tracking */
void 
tty_write(const char* str) 
{
    // loop through each character and putc to screen
    for (size_t i = 0; i < kstrlen(str); ++i) 
    {
        // detect when a newline character is present
        tty_putc(str[i]);
    }
}

void 
tty_writecolor(const char* str, vga_color_t fg, vga_color_t bg) 
{
    // set color to desired
    vga_setcolor(fg, bg);

    // loop through each character and putc to screen
    tty_write(str);

    // set color back to default tty state
    vga_setcolor(tty_state.fgcolor, tty_state.bgcolor);
}

void 
tty_putc(char c)
{
    if (c != '\n') vga_putc(c, tty_state.row, tty_state.col);

    // if we are at the end of the line (80 columns), break line
    if (++tty_state.col == VGA_WIDTH || c == '\n') 
    {
        tty_state.col = 0;

        // same for rows
        if (++tty_state.row == VGA_HEIGHT) 
        {
            vga_scroll();
            --tty_state.row;
        } 
    }

    // update cursor after string is written to tty
    vga_update_cursor(tty_state.col, tty_state.row);
}

/* Write a character to the screen relative to current tty_state */
void 
tty_putc_relative(char c, int relx, int rely, bool cursor)
{
    tty_state.col += relx;
    tty_state.row += rely;

    if (c != '\n') vga_putc(c, tty_state.row, tty_state.col);

    // update cursor after string is written to tty
    if (cursor) vga_update_cursor(tty_state.col, tty_state.row);
}

/* Clear screen and reset row, col pointers */
void 
tty_clear() 
{
    // clear vga buffer
    vga_clear();

    // reset column and row pointers
    tty_state.col = 0;
    tty_state.row = 0;
}

/* Set new color and store fg/bg colors in state */
void 
tty_setcolor(vga_color_t fg, vga_color_t bg) 
{
    vga_setcolor(fg, bg);

    tty_state.fgcolor = fg;
    tty_state.bgcolor = bg;
}

/* Just a somewhat unecessary boot success message */
void 
tty_neofetch() 
{
    // temporarily set text color to green
    vga_setcolor(VGA_COLOR_GREEN, tty_state.bgcolor);
    tty_write("\nYou're not that guy, pal..\n");

    // set text color back to default
    vga_setcolor(tty_state.fgcolor, tty_state.bgcolor);

    // some dope ascii art that I definitely didn't generate...
    tty_write("\n\n");
    tty_write(" /$$        /$$$$$$   /$$$$$$\n");
    tty_write("| $$       /$$__  $$ /$$__  $$\n");
    tty_write("| $$   /$$| $$  \\ $$| $$  \\__/\n");
    tty_write("| $$  /$$/| $$  | $$|  $$$$$$ \n");
    tty_write("| $$$$$$/ | $$  | $$ \\____  $$\n");
    tty_write("| $$_  $$ | $$  | $$ /$$  \\ $$\n");
    tty_write("| $$ \\  $$|  $$$$$$/|  $$$$$$/\n");
    tty_write("|__/  \\__/ \\______/  \\______/ \n");
    tty_write("\nv0.0.2\n");

    tty_write("\n\nWelcome to kOS!\n");

    // enable vertical cursor (max scanline 15)
    vga_cursor_enable(1, 15);
    
    // update cursor to current row, col
    vga_update_cursor(tty_state.col, tty_state.row);
}
