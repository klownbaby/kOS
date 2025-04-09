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
#include "drivers/keyboard.h"
#include "drivers/tty.h"

/* Define uppercase and lowercase char mappings */
static const uint32_t lowercase[128] = {
    UNKNOWN,ESC,'1','2','3','4','5','6','7','8',
    '9','0','-','=','\b','\t','q','w','e','r',
    't','y','u','i','o','p','[',']','\n',CTRL,
    'a','s','d','f','g','h','j','k','l',';',
    '\'','`',LSHFT,'\\','z','x','c','v','b','n','m',',',
    '.','/',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,
    '+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

static const uint32_t uppercase[128] = {
    UNKNOWN,ESC,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R',
    'T','Y','U','I','O','P','{','}','\n',CTRL,'A','S','D','F','G','H','J','K','L',':','"','~',LSHFT,'|','Z','X','C',
    'V','B','N','M','<','>','?',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',
    LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

/* Shift and caps lock state */
bool caps = false;
bool caps_lock = false;

static keyboard_notify_cb notify_cb;

static void 
keyboard_cb(__attribute__((unused)) i_register_t registers) 
{
    unsigned char scan = inb(0x60) & 0x7F;
    unsigned char pressed = inb(0x60) & 0x80;

    // we can have the parent handle some of these (us)
    switch (scan)
    {
        case _LSHIFT: //LSHIFT
        case _RSHIFT: //RSHIFT
            caps_lock = !caps_lock;
            break;
        case _CAPS: //CAPS
            caps_lock = pressed ? !caps_lock : caps_lock;
            break;
    }

    // make sure the stuff that SHOULDNT be written to the screen, doesnt.
    // this isnt fully covered yet, but its prob fine for now.
    switch (scan)
    {
        case _CTRL: //CTRL
        case _ALT: //ALT
        case _ALTGR: //ALTGR
        case _NUMLOCK: //NUMLOCK
        case _SCRLOCK: //SCRLOCK
        case _F1: //F1
        case _F2: //F2
        case _F3: //F3
        case _F4: //F4
        case _F5: //F5
        case _F6: //F6
        case _F7: //F7
        case _F8: //F8
        case _F9: //F9
        case _F10: //F10
        case _F11: //F11
        case _F12: //F12
        case _ESC: //ESC
        case _TAB: //TAB
        case _UP: //UP
        case _DOWN: //DOWN
        case _LEFT: //LEFT
        case _RIGHT: //RIGHT
        case _PGUP: //PGUP
        case _PGDOWN: //PGDOWN
        case _HOME: //HOME
        case _END: //END
        case _RSHIFT:
        case _LSHIFT:
        case _INSERT: //INSERT
        case _DELETE: //DELETE
        case _CAPS: //CAPS
            KASSERT_GOTO_SUCCESS(true);
            break;
        default:
            KASSERT_GOTO_SUCCESS(false);
            break;
    }
   

    // if no notify callback attached, we're done
    KASSERT_GOTO_SUCCESS(notify_cb == NULL);

    // call our notification callback
    notify_cb(scan, pressed);

success:
    return;
}

/* Initialize keyboard driver */
void 
keyboard_init() 
{
    // register our interrupt handler for irq1
    register_interrupt_handler(IRQ1, keyboard_cb);

    BOOT_LOG("Keyboard initialized.")
}

/* Convert a scan code into ascii char */
char
keyboard_scan_to_char(uint8_t scan)
{
    return (caps || caps_lock) ? uppercase[scan] : lowercase[scan];
}

/* Set a keypress notification callback */
void
keyboard_set_notify_cb(keyboard_notify_cb cb)
{
    // set our new notify callback
    notify_cb = cb;
}
