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

#include "hash.h"

/* Hash a string using our constant */
ULONG
HashStr(CHAR *str)
{
    // use our awful constant lol
    ULONG hash = HASH_CONSTANT;
    ULONG c = 0;
    CHAR *tmp = str;

    // hash the fucking string
    while ((c = (ULONG)*tmp++))
    {
        // wow! what a sophisticated algorithm!
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
