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


/* Callback for handling all syscalls */
static void 
syscall_cb(i_register_t registers) 
{
    KASSERT_PANIC(registers.eax > (SYSCALL_MAX - 1), "Invalid syscall!");
}

/* 
 * Begin syscall definitions
 */

/* write(), writes to a file descriptor (fd) */
static void 
__write(i_register_t* registers)
{   
    // get syscall parameters from registers struct
    int fd = registers->ebx;
    char* buffer = (char*) registers->ecx;
    size_t n = registers->edx;

    // check for standard file descriptors
}

/* open(), writes to a file descriptor (fd) */
static void 
__open(i_register_t* registers)
{
    printk("syscall open()\n");
}

/* Init syscall callbacks */
void 
syscall_init() 
{
    register_interrupt_handler(128, syscall_cb); 
}
