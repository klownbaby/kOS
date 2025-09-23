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

/* Initialize early essential modules */
static KSTATUS
modulesInit(VOID)
{
    KSTATUS status = STATUS_FAILED;
    MODULE_ENTRY *modules = NULL;
    ULONG dynmodSize = 0;
    ULONG numModules = 0;

    // get size of .dynmod section in bytes
    dynmodSize = (ULONG)((ULONG)&_dynmod_end - (ULONG)&_dynmod_start);
    KASSERT_GOTO_SUCCESS(dynmodSize == 0);

    // calculate number of entry points
    numModules = dynmodSize / sizeof(ULONG);

    // get first module entry point
    modules = (MODULE_ENTRY *)&_dynmod_start;

    for (ULONG i = 0; i < numModules; ++i)
    {
        // call each entry point
        modules[i]();
    }

success:
    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Kernel entry point (init hardware and drivers) */
VOID 
kernel_main(volatile ULONG magic, volatile MULTIBOOT_INFO *mbd) 
{
    KSTATUS status = STATUS_FAILED;

    // set GRUB multiboot info global at start
    g_Mbd = (MULTIBOOT_INFO *)mbd;

    // do early essential inits
    status = modulesInit();
    KASSERT_GOTO_FAIL_MSG(
        status != STATUS_SUCCESS,
        "Failed to initialize essential modules!");

    // print a dope ass message
    TTYNeofetch();

    // start our awful shell
    KShellInit();

fail:
    // hang
    for(;;);
}
