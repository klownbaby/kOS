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
        case 42:
        case 54:
            caps_lock = !caps_lock;
            break;
        case 58:
            caps_lock = pressed ? !caps_lock : caps_lock;
            break;
    }

    // we don't want to have the callback handle everything
    KASSERT_GOTO_SUCCESS(scan == 58 || scan == 42 || scan == 54);

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

MODULE_ENTRY_ORDERED(keyboard_init, 5);
