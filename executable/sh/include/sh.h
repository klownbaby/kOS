#pragma once

#include "ktypes.h"

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

VOID
HandleClear(ULONG argc, CHAR **argv);

VOID
HandleReboot(ULONG argc, CHAR **argv);

VOID
HandleDumpt(ULONG argc, CHAR **argv);

VOID
HandleDumpfs(ULONG argc, CHAR **argv);

VOID
HandleDumpfl(ULONG argc, CHAR **argv);

VOID
HandleNeofetch(ULONG argc, CHAR **argv);

VOID
HandlePoke(ULONG argc, CHAR **argv);

VOID
HandleProd(ULONG argc, CHAR **argv);

VOID
HandleExec(ULONG argc, CHAR **argv);

VOID
HandlePs(ULONG argc, CHAR **argv);

VOID
HandleLs(ULONG argc, CHAR **argv);

VOID
HandleCat(ULONG argc, CHAR **argv);
