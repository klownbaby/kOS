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

SIZE 
KStrLen(const CHAR *str) 
{
    // Assume length at zero
    SIZE length = 0;

    // Increment length when a CHARacter is present
    while(str[length])
    {
        ++length;
    }

    return length;
}

VOID 
KStrCopy(CHAR *dest, const CHAR *src) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (SIZE i = 0; i < KStrLen(src); ++i)
    {
        dest[i] = src[i];
    }
}

VOID 
KStrNCopy(CHAR *dest, const CHAR *src, SIZE n) 
{
    // Pretty loosey goosey way of doing this but fuck it
    for (SIZE i = 0; i < n; ++i)
    {
        if (src[i] != '\0') dest[i] = src[i];
    }
}

CHAR* 
KStrCat(CHAR *dest, const CHAR *append)
{
    // appends one string to another (concatination)
    CHAR* save = dest;
    SIZE i = 0;

	  for (; *dest; ++dest) {}
    while ((*dest++ = *append++)); 

    return save;
}

ULONG
KStrNCmp(const CHAR *s1, const CHAR *s2, SIZE n)
{
    ULONG match = 0;

    while (n && *s1 && (*s1 == *s2))
    {
        ++s1;
        ++s2;
        --n;
    }

    KASSERT_GOTO_FAIL(n == 0);
    match = (*(UCHAR *)s1 - *(UCHAR *)s2);

fail:
    return match;
}

/* Get number of tokens in string */
ULONG
KStrNTok(CHAR *str, const CHAR delim)
{
    CHAR *tmp = str;
    ULONG count = 1;

    while (*tmp++ != '\0')
    {
        if (*tmp == delim) ++count;
    }

    return count;
}

/* Get offset of next token in string */
ULONG
KStrTokOffset(CHAR *str, const CHAR delim)
{
    CHAR *tmp = str;
    ULONG offset = 0;

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
CHAR **
KStrSplit(CHAR *str, const CHAR delim, ULONG *elemCount)
{
    CHAR **tokens = NULL;
    CHAR *tmp = NULL;
    CHAR *elem = NULL;
    SIZE count = 0;
    SIZE elemSize = 0;

    tmp = str;

    count = KStrNTok(tmp, delim);
    tokens = KMalloc(sizeof(CHAR *) * count);

    for (ULONG i = 0; i < count; ++i)
    {
        // get size of next word
        elemSize = KStrTokOffset(tmp, delim);

        if (elemSize == 0) break;

        // allocate size + NULL CHARacter
        elem = KMalloc(elemSize + 1);

        // zero out buffer
        KMemSet(elem, 0, elemSize + 1);
        // copy element into buffer
        KMemCopy(elem, tmp, elemSize);

        tokens[i] = elem;

        // reset string to after last delimeter
        tmp += elemSize + 1;
    }

fail:
    *elemCount = count;

    return tokens;
}
