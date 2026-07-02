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

/* Internal fixed-size environment variable cache */
static ENV_ENTRY envVars[ENV_VAR_HASHMAP_SIZE] = {0};

/* Get an environment variable by name from the cache, if exists, or get size if outData is NULL */
KSTATUS
GetEnvVar(CHAR *name, VOID **outData, ULONG *outSize)
{
    KSTATUS status = STATUS_UNKNOWN;
    ENV_ENTRY *currentEntry = NULL;
    ULONG hash = 0;

    // sanity checks
    KASSERT_GOTO_FAIL_ERR(name == NULL, STATUS_INVALID);
    KASSERT_GOTO_FAIL_ERR((outData == NULL || outSize == NULL), STATUS_INVALID);

    // check if variable exists
    hash = HashStr(name) % ENV_VAR_HASHMAP_SIZE;
    currentEntry = &envVars[hash];
    
    // another sanity check
    KASSERT_GOTO_FAIL_ERR(currentEntry == NULL, STATUS_INSUFFICIENT_SPACE);

    // check if it exists
    KASSERT_GOTO_FAIL_ERR(currentEntry->name[0] == '\0', STATUS_NOT_FOUND);

    // set size and data output params
    *outSize = currentEntry->size;
    *outData = currentEntry->data;

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Set an environment variable, or create new if non-existant */
KSTATUS
SetEnvVar(CHAR *name, VOID *inData, ULONG inSize)
{
    KSTATUS status = STATUS_UNKNOWN;
    ENV_ENTRY *currentEntry = NULL;
    ULONG hash = 0;

    // sanity checks
    KASSERT_GOTO_FAIL_ERR((name == NULL || inSize == 0), STATUS_INVALID);
    KASSERT_GOTO_FAIL_ERR(inSize > MAX_ENV_VAR_DATA_SIZE, STATUS_INSUFFICIENT_SPACE);

    // check if variable exists
    hash = HashStr(name) % ENV_VAR_HASHMAP_SIZE;
    currentEntry = &envVars[hash];

    // another sanity check
    KASSERT_GOTO_FAIL_ERR(currentEntry == NULL, STATUS_INSUFFICIENT_SPACE);

    // handle deletion
    if (inData == NULL)
    {
        KMemSet(currentEntry, 0, sizeof(ENV_ENTRY));
        GOTO_SUCCESS;
    }

    // update if exists
    if (currentEntry->name[0] != '\0')
    {
        // ensure we zero-out existing data for clean update
        // TODO: is this fucking stupid??
        KMemSet(currentEntry->data, 0, MAX_ENV_VAR_DATA_SIZE);

    } else {
        // create new if non-existant
        KStrNCopy(currentEntry->name, name, 128);
    }

    // copy over new data
    KMemCopy(currentEntry->data, inData, inSize);

    // update with new size
    currentEntry->size = inSize;

success:
    status = STATUS_SUCCESS;

fail:
    return status;
}
