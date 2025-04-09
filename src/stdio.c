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
char* 
kitoa(int value, char* str, int base) 
{
    char* rc;
    char* ptr;
    char* low;

    unsigned int ucast;
    
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
        char tmp = *low;

        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}

/* Put string */
void 
kputs(const char* str)
{
    tty_write(str);
}

/* Kernel printf targeting tty output */
void 
printk(const char* fmt, ...) 
{
    // init args and string buffer
    va_list ap;
    char* buffer;
    char c;
    int i;

    va_start(ap, fmt);

    // loop through string
    for(i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        // if token not found, write raw character
        if (c != '%') {
            tty_putc(c);
            continue;
        }

        // increment character pointer
        c = fmt[++i] & 0xff;

        // check for null termination
        if (c == 0) break;

        switch(c) {
            case 'd':
                // handle int to ascii conversion
                kitoa(va_arg(ap, int), buffer, DECIMAL);

                // write filled buffer
                tty_write(buffer);
                break;
            case 'x':
                // handle hex conversion
                kitoa(va_arg(ap, int), buffer, HEX);
                
                // write prefix "0x" with filled buffer
                tty_write("0x");
                tty_write(buffer);
                break;
            case 'c':
                // write character passed
                tty_putc(va_arg(ap, int));
                break;
            case 's':
                // convert arg to string (char*) and write to tty
                tty_write(va_arg(ap, char*));
                break;
            case '%':
                // handle double percent (i.e. printing a % symbol)
                tty_write("%");
                break;
            default:
                // otherwise, just write the fucking character
                tty_write("%");
                tty_write(&c);
                break;
        }
    }
}

/* WIP */
void 
sprintk(const char* fmt, char* buffer, ...) 
{
    // init args and string buffer
    va_list ap;
    char c;
    int i;

    va_start(ap, fmt);

    // loop through string
    for(i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        // if token not found, write raw character
        if (c != '%') {
            buffer[i] = c;
            continue;
        }

        // increment character pointer
        c = fmt[++i] & 0xff;

        // check for null termination
        if (c == 0) break;

        switch(c) {
            case 'd':
                // handle int to ascii conversion
                kitoa(va_arg(ap, int), buffer, DECIMAL);
                break;
            case 'x':
                {
                    char* tmp;
                    // handle hex conversion
                    kitoa(va_arg(ap, int), tmp, HEX);

                    kstrcat(buffer, tmp); 
                    break;
                }
            case 'c':
                // write character passed
                buffer[i] = (char) va_arg(ap, int);
                break;
            case 's':
                // convert arg to string (char*) and write to tty
                {
                    char* tmp = va_arg(ap, char*);
                    break;
                }
            case '%':
                // handle double percent (i.e. printing a % symbol)
                buffer[i] = '%';
                break;
            default:
                // otherwise, just write the fucking character
                buffer[i] = c;
                break;
        }
    }
}

/* Kernel memset, nearly identical to glibc implementation */
void
kmemset(void* dest, register int data, register size_t length) 
{
    // Cast destination pointer to char pointer to dereference later
    unsigned char* ptr = (unsigned char*)dest;
    size_t i;

    // Loop through each byte until end of data
    for (i = 0; i < length; ++i) 
    {
        // Set dereferenced destination to data at index
        *ptr++ = data;
    }
}

int 
kmemcmp(const void* aptr, const void* bptr, size_t size) {
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}

void
kmemcpy(void* dest, void* src, register size_t size)
{
    unsigned char* dptr = (unsigned char*)dest;
    unsigned char* sptr = (unsigned char*)src;
    size_t i;

    for (i = 0; i < size; ++i)
    {
        // do the copy
        *dptr++ = *sptr++;
    }
}

/* Kernel panic/exception handler, nothing fancy */
__attribute__((noreturn)) void 
kpanic(char* msg)
{
    vga_setcolor(VGA_COLOR_RED, VGA_COLOR_BLACK);
    // generic error handler
    printk("Exception encountered! %s\n", msg);

    // halt all execution
    hlt();
}
