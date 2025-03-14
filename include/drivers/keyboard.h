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
#include "ktypes.h"

#define UNKNOWN 0xFFFFFFFF
#define ESC 0xFFFFFFFF - 1
#define CTRL 0xFFFFFFFF - 2
#define LSHFT 0xFFFFFFFF - 3
#define RSHFT 0xFFFFFFFF - 4
#define ALT 0xFFFFFFFF - 5
#define F1 0xFFFFFFFF - 6
#define F2 0xFFFFFFFF - 7
#define F3 0xFFFFFFFF - 8
#define F4 0xFFFFFFFF - 9
#define F5 0xFFFFFFFF - 10
#define F6 0xFFFFFFFF - 11
#define F7 0xFFFFFFFF - 12
#define F8 0xFFFFFFFF - 13
#define F9 0xFFFFFFFF - 14
#define F10 0xFFFFFFFF - 15
#define F11 0xFFFFFFFF - 16
#define F12 0xFFFFFFFF - 17
#define SCRLCK 0xFFFFFFFF - 18
#define HOME 0xFFFFFFFF - 19
#define UP 0xFFFFFFFF - 20
#define LEFT 0xFFFFFFFF - 21
#define RIGHT 0xFFFFFFFF - 22
#define DOWN 0xFFFFFFFF - 23
#define PGUP 0xFFFFFFFF - 24
#define PGDOWN 0xFFFFFFFF - 25
#define END 0xFFFFFFFF - 26
#define INS 0xFFFFFFFF - 27
#define DEL 0xFFFFFFFF - 28
#define CAPS 0xFFFFFFFF - 29
#define NONE 0xFFFFFFFF - 30
#define ALTGR 0xFFFFFFFF - 31
#define NUMLCK 0xFFFFFFFF - 32

/* Keyboard function defs */
void 
keyboard_init();

char
keyboard_scan_to_char(uint8_t scan);

void
keyboard_set_notify_cb(keyboard_notify_cb cb);
