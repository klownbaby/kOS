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
#include "drivers/tty.h"
#include "drivers/fat.h"

/* Handle clear ksh command (clear screen) */
VOID
HandleClear(ULONG argc, CHAR **argv)
{
    TTYClear();
}

/* Handle reboot ksh command (warm reboot) */
VOID
HandleReboot(ULONG argc, CHAR **argv)
{
    __warmReboot();
}

/* Handle dumpt ksh command (dump page tables) */
VOID
HandleDumpt(ULONG argc, CHAR **argv)
{
    PmmDumpPageDir();
}

/* Handle dumpfs ksh command (dump FAT BIOS parameter block) */
VOID
HandleDumpfs(ULONG argc, CHAR **argv)
{
    FatDumpBs();
}

/* Handle dumpfl ksh command (dump free list) */
VOID
HandleDumpfl(ULONG argc, CHAR **argv)
{
    DumpFreeList();
}

/* Handle neofetch ksh command (dumb lol) */
VOID
HandleNeofetch(ULONG argc, CHAR **argv)
{
    TTYNeofetch();
}

/* Poke (read) from a given memory address */
VOID
HandlePoke(ULONG argc, CHAR **argv)
{
    ULONG addr = 0;
    ULONG size = 0;
    UINT8 *buffer = NULL;

    KASSERT_GOTO_FAIL_MSG(argc < 3, "Usage: poke [address] [size]\n");

    // get address and size from args
    addr = KAToI(argv[1]);
    size = KAToI(argv[2]);

    // for now, restrict size to 32 bytes
    KASSERT_GOTO_FAIL_MSG(size > 0x10, "Inavlid size, must be less than 32!\n");

    // allocate a scratch buffer
    buffer = KMalloc(size);

    // copy desired size
    KMemCopy(buffer, (VOID *)addr, size);

    // finally, dump
    KPrint("Dumping at address (0x%x)\n", addr);
    KPrint("    { ");

    for (ULONG i = 0; i < size; ++i)
    {
        KPrint("0x%x, ", buffer[i]);
    }

    KPrint("}\n");

fail:
    // ensure we free our scratch buffer
    if (buffer) KFree(buffer);

    return;
}

/* Prod (write) to a given memory address */
VOID
HandleProd(ULONG argc, CHAR **argv)
{
    PROC_HANDLE handle = { 0 };
    VOID *buffer = NULL;    

    buffer = KMalloc(32);

    (VOID)ProcLoad(buffer, 32);

    KFree(buffer);
}

VOID
HandleLs(ULONG argc, CHAR **argv)
{
    VOID *dir = NULL;
    ULONG size = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: ls [path]\n");

    // should we just dump the root directory?
    if (argv[1][0] == '/' && argv[1][1] == '\0')
    {
        FatDumpRoot();        
        GOTO_SUCCESS;
    }

    // if not, find the requested directory
    dir = FatOpen(argv[1], &size);
    KASSERT_GOTO_FAIL_MSG(dir == NULL, "Path not found!\n");

    // finally, dump that JAWN
    FatDumpDirectory(dir);

fail:
    if (dir)
    {
        KFree(dir);
    }

success:
    return;
}

VOID
HandleCat(ULONG argc, CHAR **argv)
{
    CHAR *data = NULL;
    ULONG size = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: cat [file name]\n");

    // open a handle (buffer) to our file
    data = FatOpen(argv[1], &size);
    KASSERT_GOTO_FAIL_MSG(data == NULL, "File not found!\n");

    // just dump as if all data is valid ASCII for now
    for (ULONG i = 0; i < size; ++i)
    {
        KPrint("%c", data[i]);
    }

fail:
    // ensure we free our buffer
    if (data)
    {
        KFree(data);
    }

    return;
}
