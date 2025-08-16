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
uint32_t
hashstr(char *str)
{
    // use our awful constant lol
    uint32_t hash = HASH_CONSTANT;
    uint32_t c = 0;
    char *tmp = str;

    // hash the fucking string
    while ((c = (uint32_t)*tmp++))
    {
        // wow! what a sophisticated algorithm!
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
