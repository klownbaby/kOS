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

#include <stdarg.h>
#include "kernel.h"
#include "drivers/tty.h"
#include "drivers/vga.h"

/* Converts int to ascii representation (used for printing) */
CHAR * 
KIToA(int value, CHAR *str, int base) 
{
    CHAR *rc;
    CHAR *ptr;
    CHAR *low;
    CHAR tmp;
    ULONG ucast;
    
    // store unsigned value for later
    ucast = value;

    // check for supported base.
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }

    rc = ptr = str;

    // set '-' for negative decimals.
    if (value < 0 && base == 10) *ptr++ = '-';

    // remember where the numbers start.
    low = ptr;

    do {
        // had to use this dirty trick for signed 32 bit integers
        if (base == HEX && value < 0) {
            *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + ucast % base];

            ucast /= base;
        } else {
            // modulo is negative for negative value. This trick makes abs() unnecessary.
            *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];

            value /= base;
        }
    } while (ucast && value);

    // terminating the string.
    *ptr-- = '\0';

    // invert the numbers.
    while (low < ptr) {
        tmp = *low;

        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}

/* Convert ASCII string to integer */
ULONG
KAToI(const CHAR *str)
{
    ULONG k = 0;

    while (*str)
    {
        k = (k << 3) + (k << 1) + (*str) - '0';
        str++;
    }

    return k;
}

/* Put string */
VOID 
KPutS(const CHAR *str)
{
    TTYWrite(str);
}

/* Kernel printf targeting tty output */
VOID 
KPrint(const CHAR *fmt, ...) 
{
    // init args and string buffer
    va_list ap;
    CHAR *buffer;
    CHAR c;
    int i;

    va_start(ap, fmt);

    // loop through string
    for(i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        // if token not found, write raw CHARacter
        if (c != '%') {
            TTYPutC(c);
            continue;
        }

        // increment CHARacter pointer
        c = fmt[++i] & 0xff;

        // check for null termination
        if (c == 0) break;

        switch(c) {
            case 'd':
                // handle int to ascii conversion
                KIToA(va_arg(ap, int), buffer, DECIMAL);

                // write filled buffer
                TTYWrite(buffer);
                break;
            case 'x':
                // handle hex conversion
                KIToA(va_arg(ap, int), buffer, HEX);
                
                // write filled buffer
                TTYWrite(buffer);
                break;
            case 'c':
                // write CHARacter passed
                TTYPutC(va_arg(ap, int));
                break;
            case 's':
                // convert arg to string (CHAR*) and write to tty
                TTYWrite(va_arg(ap, CHAR*));
                break;
            case '%':
                // handle double percent (i.e. printing a % symbol)
                TTYWrite("%");
                break;
            default:
                // otherwise, just write the fucking CHARacter
                TTYWrite("%");
                TTYWrite(&c);
                break;
        }
    }
}

/* WIP */
VOID 
KSPrint(const CHAR *fmt, CHAR *buffer, ...) 
{
    // init args and string buffer
    va_list ap;
    CHAR *tmp;
    CHAR c;
    int i;

    va_start(ap, fmt);

    // loop through string
    for(i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        // if token not found, write raw CHARacter
        if (c != '%') {
            buffer[i] = c;
            continue;
        }

        // increment CHARacter pointer
        c = fmt[++i] & 0xff;

        // check for null termination
        if (c == 0) break;

        switch(c) {
            case 'd':
                // handle int to ascii conversion
                KIToA(va_arg(ap, int), buffer, DECIMAL);
                break;
            case 'x':
                // handle hex conversion
                KIToA(va_arg(ap, int), tmp, HEX);

                KStrCat(buffer, tmp); 
                break;
            case 'c':
                // write CHARacter passed
                buffer[i] = (CHAR)va_arg(ap, int);
                break;
            case 's':
                // convert arg to string (CHAR*) and write to tty
                tmp = va_arg(ap, CHAR *);
                break;
            case '%':
                // handle double percent (i.e. printing a % symbol)
                buffer[i] = '%';
                break;
            default:
                // otherwise, just write the fucking CHARacter
                buffer[i] = c;
                break;
        }
    }
}

/* Kernel memset, nearly identical to glibc implementation */
VOID
KMemSet(VOID *dest, register int data, register SIZE length) 
{
    // Cast destination pointer to CHAR pointer to dereference later
    UINT8 *ptr = (UINT8 *)dest;
    SIZE i;

    // Loop through each byte until end of data
    for (i = 0; i < length; ++i) 
    {
        // Set dereferenced destination to data at index
        *ptr++ = data;
    }
}

VOID
KMemCopy(VOID *dest, VOID *src, register SIZE size)
{
    UINT8 *dptr = (UINT8 *)dest;
    UINT8 *sptr = (UINT8 *)src;
    SIZE i = 0;

    for (i = 0; i < size; ++i)
    {
        // do the copy
        *dptr++ = *sptr++;
    }
}

/* Kernel panic/exception handler, nothing fancy */
__attribute__((noreturn)) VOID 
KPanic(CHAR *msg)
{
    VgaSetColor(VGA_COLOR_RED, VGA_COLOR_BLACK);

    // generic error handler
    KPrint("Exception encountered! %s\n", msg);

    // halt all execution
    __hlt();
}
