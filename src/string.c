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

#include <stddef.h>
#include "kernel.h"
#include "kutils.h"

size_t 
kstrlen(const char* str) 
{
    // Assume length at zero
    size_t length = 0;

    // Increment length when a character is present
    while(str[length])
        ++length;

    return length;
}

void 
kstrcpy(char* dest, const char* src) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (size_t i = 0; i < kstrlen(src); ++i) {
        dest[i] = src[i];
    }
}

void 
kstrncpy(char* dest, const char* src, size_t n) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (size_t i = 0; i < n; ++i) {
        if (src[i] != '\0') dest[i] = src[i];
    }
}

char* 
kstrcat(char* dest, const char* append)
{
    // appends one string to another (concatination)
    char* save = dest;
    size_t i;

	for (; *dest; ++dest);
	while ((*dest++ = *append++)); 

	return save;
}

bool 
kstrcmp(const char* a, const char* b) 
{
    // Cache length of a and b
    size_t a_len = kstrlen(a);
    size_t b_len = kstrlen(b);

    // If lengths don't match, then strings are not the same
    if (a_len != b_len) return false;

    // Loop through each character and compare
    for (size_t i = 0; i < a_len; ++i) {
        if (a[i] == b[i]) continue;
        // Fail check if any are different
        return false;
    }

    return true;
}

/* This is kinda awful (and dangerous!)... but it works for now, TODO: Fix plz */
char **
kstrsplit(char *str, const char delim, uint32_t *elem_count)
{
    char **tokens = NULL;
    char *tmp = NULL;
    size_t size = 0;
    size_t elem_size = 0;
    uint32_t elem_start = 0;
    uint32_t current_offset = 0;
    uint32_t count = 1;
    uint32_t i = 0;

    // get length of raw string
    size = kstrlen(str);
    tmp = str;

    // first iteration, count delimeters
    while (*tmp++ != '\0')
    {
        if (*tmp == delim) ++count;
    }

    // allocate buffer for string list
    tokens = (char **)kmalloc(sizeof(char *) * count);
    kmemset(tokens, 0, sizeof(char *) * count);

    // reset count
    count = 0;

    for (uint32_t i = 0; i <= size; ++i)
    {
        if (str[i] == delim || str[i] == '\0')
        {
            // get current offset within string and size
            current_offset = (uint32_t)(&str[i]);
            elem_size = current_offset - (uint32_t)&str[elem_start];

            // allocate buffer for string element (plus NULL)
            // REMEMBER, the CALLER OWNS THIS BUFFER!
            tmp = kmalloc(elem_size + 1);

            // zero out buffer
            kmemset(tmp, 0, elem_size + 1);
            kmemcpy(tmp, (void *)(&str[i] - elem_size), elem_size);

            // set element in list, increment list counter
            tokens[count++] = tmp;

            // reset start pointer to just after delimeter
            elem_start = i + 1;
        }
    }

success:
    // write count to out pointer
    *elem_count = count;

    // the CALLER now OWNS THIS BUFFER TOO!
    return tokens;
}
