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
#include "kutils.h"

/* Insert entry at tail of linked list */
VOID
ListAddTail(VOID **head, VOID *entry)
{
    LIST_ENTRY *currentHead = NULL;
    LIST_ENTRY *newEntry = NULL;
    LIST_ENTRY *tail = NULL;

    currentHead = *((LIST_ENTRY **)head);
    newEntry = (LIST_ENTRY *)entry;

    // if list head exists append tail
    if (currentHead)
    {
        tail = currentHead->prev;
    }
    else
    {
        currentHead = newEntry;
        tail = newEntry;

        *head = currentHead;
    }

    // insert after current tail
    newEntry->prev = tail;
    newEntry->next = currentHead;

    // update surrounding links
    newEntry->next->prev = newEntry;
    newEntry->prev->next = newEntry;
}

/* Remove an entry from a linked list */
VOID
ListDeleteEntry(VOID **head, VOID *entry)
{
    
    LIST_ENTRY *currentHead = NULL;
    LIST_ENTRY *toDelete = NULL;
    LIST_ENTRY *tail = NULL;

    // ensure list is valid
    currentHead = *((LIST_ENTRY **)head);
    KASSERT_GOTO_FAIL_MSG(currentHead == NULL, "List not initialized\n!");

    toDelete = (LIST_ENTRY *)entry;

    // if we're the head, just set to NULL
    if (toDelete == currentHead)
    {
        currentHead = NULL;
        *head = currentHead;
    }
    else
    {
        toDelete->next->prev = toDelete->prev;
        toDelete->prev->next = toDelete->next;

        toDelete->next = NULL;
        toDelete->prev = NULL;
    }

fail:
    return;
}
