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
    (sizeof(cmdHashmap) / sizeof(CMD_HANDLER))
#define CMD_LIST_SIZE \
    (sizeof(cmdHandlers) / sizeof(CMD_HANDLER))

/* Command processor (callback) */
typedef VOID (*CMD_PROCESSOR)(ULONG argc, CHAR **argv);

/* Defining a private struct for command handlers */
typedef struct _CMD_HANDLER {
    CHAR *cmdstr;
    CMD_PROCESSOR proc;
} CMD_HANDLER;

/* Shell function defs */
VOID
KShellInit(VOID);

VOID
KShellFini(VOID);
