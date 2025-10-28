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
#include "string.h"

VOID *
ResolveKSym(CHAR *name)
{
    VOID *funcAddress = NULL;

    // for now just assume 100 entries and iterate
    for (ULONG i = 0; i < 100; ++i)
    {
        // eventually this will be a hashmap
        if (KStrNCmp(name, g_KernelSyms[i].name, KStrLen(name)) == 0)
        {
            funcAddress = g_KernelSyms[i].funcAddress;
            break;
        }
    }

    return funcAddress;
}
