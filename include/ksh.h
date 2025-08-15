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

#pragma once

#include <stdint.h>

/* For now, max input buffer size */
#define KSH_INPUTBUF_SIZE 0xFF

/* We need our hashmap and command list sizes at compile time */
#define HASHMAP_SIZE \
    (sizeof(cmd_hashmap) / sizeof(cmd_handler_t))
#define CMD_LIST_SIZE \
    (sizeof(cmd_handlers) / sizeof(cmd_handler_t))

/* Command processor (callback) */
typedef void (*cmd_proc_t)(uint32_t argc, char **argv);

/* Defining a private struct for command handlers */
typedef struct cmd_handler {
    char *cmdstr;
    cmd_proc_t proc;
} cmd_handler_t;

/* Shell function defs */
void
ksh_init();

void
ksh_fini();
