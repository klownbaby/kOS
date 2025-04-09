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

// define shit that should NOT be rendered as text
#define _CAPS 58
#define _RSHIFT 42
#define _LSHIFT 54
#define _CTRL 29
#define _ALT 56
#define _ALTGR 100
#define _NUMLOCK 69
#define _SCRLOCK 70
#define _F1 59
#define _F2 60
#define _F3 61
#define _F4 62
#define _F5 63
#define _F6 64
#define _F7 65
#define _F8 66
#define _F9 67
#define _F10 68
#define _F11 87
#define _F12 88
#define _ESC 1
#define _TAB 15
#define _UP 72
#define _DOWN 80
#define _LEFT 75
#define _RIGHT 77
#define _PGUP 73
#define _PGDOWN 81
#define _HOME 71
#define _END 79
#define _INSERT 82
#define _DELETE 83

/* Keyboard function defs */
void 
keyboard_init();

char
keyboard_scan_to_char(uint8_t scan);

void
keyboard_set_notify_cb(keyboard_notify_cb cb);
