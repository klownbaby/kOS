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
#include "drivers/rs/rust_driver.h"
/* Kernel entry point (init hardware and drivers) */
void 
kernel_main(__attribute__((used)) uint32_t magic, volatile multiboot_info_t* mbd) 
{
    // init kernel tty first
    tty_init();

    // check GRUB version
    tty_writecolor("Booted with ", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    tty_writecolor((const char*)mbd->boot_loader_name, VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    tty_write("\n\n");

    // essential inits
    gdt_init(); 
    idt_init();
    rtc_init();
    pit_init();
    keyboard_init();

    // init physical memory manager
    pmm_init(mbd);

    // init heap
    kmalloc_init();

    // init syscalls after interrupts init
    syscall_init();

    // fuck rust but we need some TESTING!
    __init();
    // print a dope ass message
    tty_neofetch();

    ksh_init();

    // hang
    for(;;);
}
