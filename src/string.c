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

size_t 
kstrlen(const char *str) 
{
    // Assume length at zero
    size_t length = 0;

    // Increment length when a character is present
    while(str[length])
    {
        ++length;
    }

    return length;
}

void 
kstrcpy(char *dest, const char *src) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (size_t i = 0; i < kstrlen(src); ++i)
    {
        dest[i] = src[i];
    }
}

void 
kstrncpy(char *dest, const char *src, size_t n) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (size_t i = 0; i < n; ++i)
    {
        if (src[i] != '\0') dest[i] = src[i];
    }
}

char* 
kstrcat(char *dest, const char *append)
{
    // appends one string to another (concatination)
    char* save = dest;
    size_t i = 0;

	  for (; *dest; ++dest) {}
    while ((*dest++ = *append++)); 

    return save;
}

uint32_t
kstrncmp(const char *s1, const char *s2, size_t n)
{
    uint32_t match = 0;

    while (n && *s1 && (*s1 == *s2))
    {
        ++s1;
        ++s2;
        --n;
    }

    KASSERT_GOTO_FAIL(n == 0);
    match = ( *(unsigned char *)s1 - *(unsigned char *)s2 );

fail:
    return match;
}

/* Get number of tokens in string */
uint32_t
kstrntok(char *str, const char delim)
{
    char *tmp = str;
    uint32_t count = 1;

    while (*tmp++ != '\0')
    {
        if (*tmp == delim) ++count;
    }

    return count;
}

/* Get offset of next token in string */
uint32_t
kstrtokoff(char *str, const char delim)
{
    char *tmp = str;
    uint32_t offset = 0;

    while (*tmp++ != '\0')
    {
        if (*tmp == delim)
        {
            ++offset;
            break;            
        }

        ++offset;
    }

    return offset;
}

/* This is kinda awful (and dangerous!)... but it works for now, TODO: Fix plz */
char **
kstrsplit(char *str, const char delim, uint32_t *elem_count)
{
    char **tokens = NULL;
    char *tmp = NULL;
    char *elem = NULL;
    size_t count = 0;
    size_t elem_size = 0;

    tmp = str;

    count = kstrntok(tmp, delim);
    tokens = kmalloc(sizeof(char *) * count);

    for (uint32_t i = 0; i < count; ++i)
    {
        // get size of next word
        elem_size = kstrtokoff(tmp, delim);

        if (elem_size == 0) break;

        // allocate size + NULL character
        elem = kmalloc(elem_size + 1);

        // zero out buffer
        kmemset(elem, 0, elem_size + 1);
        // copy element into buffer
        kmemcpy(elem, tmp, elem_size);

        tokens[i] = elem;

        // reset string to after last delimeter
        tmp += elem_size + 1;
    }

fail:
    *elem_count = count;

    return tokens;
}
