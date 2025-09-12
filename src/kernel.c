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
#include "drivers/rs/rust_driver.h"
/* Initialize early essential modules */
static kstatus_t
modules_init(void)
{
    kstatus_t status = STATUS_FAILED;
    module_entry_t *modules = NULL;
    uint32_t dynmod_size = 0;
    uint32_t num_modules = 0;

    // get size of .dynmod section in bytes
    dynmod_size = (uint32_t)((uint32_t)&_dynmod_end - (uint32_t)&_dynmod_start);
    KASSERT_GOTO_SUCCESS(dynmod_size == 0);

    // calculate number of entry points
    num_modules = dynmod_size / sizeof(uint32_t);

    // get first module entry point
    modules = (module_entry_t *)&_dynmod_start;

    for (uint32_t i = 0; i < num_modules; ++i)
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
void 
kernel_main(volatile uint32_t magic, volatile multiboot_info_t *mbd) 
{
    kstatus_t status = STATUS_FAILED;

    // set GRUB multiboot info global at start
    g_mbd = (multiboot_info_t *)mbd;

    // do early essential inits
    status = modules_init();
    KASSERT_GOTO_FAIL_MSG(
        status != STATUS_SUCCESS,
        "Failed to initialize essential modules!");
    // fuck rust but we need some TESTING!
    __init();
    // print a dope ass message
    tty_neofetch();

    // start our awful shell
    ksh_init();

fail:
    // hang
    for(;;);
}
