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
#include "drivers/keyboard.h"
#include "drivers/rtc.h"
#include "drivers/pit.h"
#include "drivers/fat.h"

/* Initialize early essential modules */
static KSTATUS
modulesInit(VOID)
{
    KSTATUS status = STATUS_FAILED;
    MODULE_ENTRY *modules = NULL;
    ULONG dynmod_size = 0;
    ULONG num_modules = 0;

    // get size of .dynmod section in bytes
    dynmod_size = (ULONG)((ULONG)&_dynmod_end - (ULONG)&_dynmod_start);
    KASSERT_GOTO_SUCCESS(dynmod_size == 0);

    // calculate number of entry points
    num_modules = dynmod_size / sizeof(ULONG);

    // get first module entry point
    modules = (MODULE_ENTRY *)&_dynmod_start;

    for (ULONG i = 0; i < num_modules; ++i)
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
