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
#include <stdbool.h>
#include "vga.h"

#define BOOT_LOG(msg) \
    tty_write("[ "); \
    tty_writecolor("OK", VGA_COLOR_GREEN, VGA_COLOR_BLACK); \
    tty_write(" ] "); \
    tty_write(msg); \
    tty_write("\n");

typedef struct {
    size_t      row;
    size_t      col;

    vga_color_t fgcolor;
    vga_color_t bgcolor;
} tty_state_t;

/* TTY function defs */
void 
tty_init();
void 
tty_write(const char* str);
void 
tty_writecolor(const char* str, vga_color_t fg, vga_color_t bg);
void 
tty_putc(char c);
void 
tty_putc_relative(char c, int relx, int rely, bool cursor);
void 
tty_clear();
void 
tty_setcolor(vga_color_t fg, vga_color_t bg);
void 
tty_neofetch();
