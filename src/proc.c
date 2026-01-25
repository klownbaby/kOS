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

#include "proc.h"
#include "kernel.h"
#include "ktypes.h"
#include "kutils.h"
#include "list.h"
#include "pmm.h"

/* Resolves all image imports, requires KPROCESS_BASE to be mapped */
static KSTATUS
resolveImports(IMAGE_IMPORT_DESCRIPTOR *importDesc)
{
    KSTATUS status = STATUS_UNKNOWN;
    IMAGE_THUNK_DATA *thunk = NULL;
    IMAGE_THUNK_DATA *orgThunk = NULL;
    IMAGE_IMPORT_BY_NAME *importByName = NULL;
    VOID *funcAddress = NULL;
    CHAR funcName[30] = { 0 };

    thunk = (IMAGE_THUNK_DATA *)((ULONG)KPROCESS_BASE + importDesc->firstThunk);
    orgThunk = (IMAGE_THUNK_DATA *)((ULONG)KPROCESS_BASE + importDesc->u.originalFirstThunk);

    for (ULONG i = 0; thunk[i].u.addressOfData != 0; ++i)
    {
        importByName = (IMAGE_IMPORT_BY_NAME *)((ULONG)KPROCESS_BASE +
                        orgThunk[i].u.addressOfData);

        // copy funciton name to temp variable
        KStrCopy(funcName, importByName->name);

        // lookup function address
        funcAddress = ResolveKSym(importByName->name);
        KASSERT_GOTO_FAIL_ERR_MSG(
            funcAddress == NULL,
            STATUS_NOT_FOUND,
            "Could not resolve imported symbol!\n");

        thunk[i].u.function = (ULONG)funcAddress;
    }

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Initialize process handle */
KSTATUS
ProcInit(PROC_HANDLE **outHandle)
{
    KSTATUS status = STATUS_UNKNOWN;
    PROC_HANDLE *handle = NULL;
    PROC_HANDLE *procListTail = NULL;

    // allocate new process handle
    handle = KMalloc(sizeof(PROC_HANDLE));
    KASSERT_GOTO_FAIL_ERR(
        handle == NULL, STATUS_INSUFFICIENT_SPACE);

    ListAddTail(&g_ProcessListHead, handle);

    // set out param
    *outHandle = handle;

    status = STATUS_SUCCESS;
    GOTO_SUCCESS;

fail:
    if (handle)
    {
        KFree(handle);
    }

success:
    return status;
}

/* Load a process */
KSTATUS
ProcLoad(PROC_HANDLE *handle)
{
    KSTATUS status =  STATUS_UNKNOWN;
    ULONG procPage = 0;
    ULONG cr3Phys = 0;
    ULONG prevCr3Phys = 0;
    ULONG *cr3Virt = NULL;
    ULONG sizeOfImage = 0;
    ULONG sizeOfHeaders = 0;
    ULONG importRva = 0;
    IMAGE_DOS_HEADER *dosHeader = NULL;
    IMAGE_PE_HEADER *peHeader = NULL;
    IMAGE_SECTION_HEADER *sections = NULL;
    IMAGE_IMPORT_DESCRIPTOR *importDesc = NULL;

    // ensure this bitch is not NULL
    KASSERT_GOTO_FAIL_ERR(
        handle == NULL, STATUS_INSUFFICIENT_SPACE);

    // size must be aligned on page boundary
    if (!IS_PAGE_ALIGNED(handle->size))
    {
        // align our size up on page boundary
        handle->size = PAGE_ALIGN_UP(handle->size);
    }

    // read PE header section mappings/virtual size of image
    dosHeader = (IMAGE_DOS_HEADER *)handle->buffer;
    peHeader = (IMAGE_PE_HEADER *)((ULONG)dosHeader + (dosHeader->e_lfanew));

    // get all sections
    sections = (IMAGE_SECTION_HEADER *)((ULONG)&peHeader->optionalHeader +
                   (ULONG)peHeader->fileHeader.sizeOfOptionalHeader);

    // virtual size of image/headers
    sizeOfImage = PAGE_ALIGN_UP(peHeader->optionalHeader.sizeOfImage);
    sizeOfHeaders = peHeader->optionalHeader.sizeOfHeaders;

    // get start vaddr of kprocess page directory list
    cr3Virt = (ULONG *)g_NextProcessPdVirt;

    // allocate and map physical page for new page directory
    cr3Phys = PmmAllocNext();
    KASSERT_GOTO_FAIL_MSG(
        cr3Phys == 0,
        "Failed to allocate physical page for kprocess page directory\n");

    // map process page directory in kernel page directory
    PmmMapPage(g_KernelPageDir, cr3Phys, (ULONG)cr3Virt);

    // increment next process cr3 vaddr
    g_NextProcessPdVirt += PAGE_SIZE;

    // map kernel into process address space
    KMemCopy(cr3Virt, g_KernelPageDir, PAGE_SIZE);

    // reset recursive mapping to new page directory
    cr3Virt[PT_VADDR_BASE >> 22] = (ULONG)cr3Phys | PAGE_PRESENT | PAGE_WRITE;

    // save current CR3
    prevCr3Phys = __getCr3();

    // set new cr3 temporarily
    __setCr3(cr3Phys);

    // start mapping process address space
    for (ULONG i = 0; i < sizeOfImage; i += PAGE_SIZE)
    {
        // allocate new physical page
        procPage = PmmAllocNext();

        KASSERT_GOTO_FAIL_MSG(
            procPage == 0,
            "Failed to allocate physical pages for process\n");

        // map a new page
        PmmMapPage(
            cr3Virt,
            procPage,
            (KPROCESS_BASE + i));
    }

    // zero out image address space
    KMemSet((VOID *)KPROCESS_BASE, 0, sizeOfImage);

    // copy PE header to process base
    KMemCopy((VOID *)KPROCESS_BASE, handle->buffer, sizeOfHeaders);

    // map sections accordingly
    for (ULONG i = 0; i < peHeader->fileHeader.numberOfSections; ++i)
    {
        KMemCopy(
            (VOID *)((ULONG)KPROCESS_BASE + sections[i].virtualAddress),
            (VOID *)((ULONG)handle->buffer + (ULONG)sections[i].pointerToRawData),
            sections[i].sizeOfRawData);
    }

    // get import descriptor
    importRva = peHeader->optionalHeader.dataDirectory[2].virtualAddress;
    importDesc = (IMAGE_IMPORT_DESCRIPTOR *)((ULONG)KPROCESS_BASE + importRva);

    // resolve necessary imports
    status = resolveImports(importDesc);
    KASSERT_GOTO_FAIL(status != STATUS_SUCCESS);

    // set proc entry
    handle->entry = (PROC_ENTRY)((ULONG)KPROCESS_BASE +
                        (ULONG)peHeader->optionalHeader.addressOfEntryPoint);

    // finally, map process meta data
    PmmMapPage(
        cr3Virt,
        PmmAllocNext(),
        KPROCESS_DATA);

    KMemCopy((VOID *)KPROCESS_DATA, handle, sizeof(PROC_HANDLE));

    // restore previous CR3
    __setCr3(prevCr3Phys);

    // set process handle members
    handle->cr3 = cr3Phys;

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Execute loaded process */
KSTATUS
ProcExec(PROC_HANDLE *handle)
{
    KSTATUS status = STATUS_UNKNOWN;
    ULONG prevCr3 = 0;
    LONG procRet = 0;

    // save current CR3
    prevCr3 = __getCr3();

    // switch to per-proccess address space
    __setCr3(handle->cr3);

    // call process entry
    procRet = handle->entry();

    // swap cr3 back to kernel address space
    __setCr3(prevCr3);

    KPrint("Process returned %x\n", procRet);

    return status;
}

/* Close open process and free all process metadata */
KSTATUS
ProcClose(PROC_HANDLE *handle)
{
    KSTATUS status = STATUS_UNKNOWN;

    // delete entry from list
    ListDeleteEntry(&g_ProcessListHead, handle);

    // free process buffer if present
    if (handle->buffer)
    {
        KFree(handle->buffer);
    }

fail:
    return status;
}

/* Get current process handle */
KSTATUS
ProcCurrent(PROC_HANDLE *handle)
{
    KSTATUS status = STATUS_UNKNOWN;
    PROC_HANDLE *current = NULL;

    // check that we are executing from a valid process context
    // jesus, this is awful TODO: change this plzz
    KASSERT_GOTO_FAIL_ERR(
        PmmVirtToPhys((ULONG *)KPROCESS_CR3_VIRT, KPROCESS_CR3_VIRT) < 0,
        STATUS_NOT_FOUND);

    // get current process meta data
    current = (PROC_HANDLE *)KPROCESS_DATA;

    // copy data to out pointer
    KMemCopy(handle, current, sizeof(PROC_HANDLE));

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Dump currently active processes to TTY */
VOID
ProcDumpList()
{
    PROC_HANDLE *current = NULL;

    // get current process list head
    current = (PROC_HANDLE *)g_ProcessListHead;
    KASSERT_GOTO_FAIL_MSG(current == NULL, "Process list empty!\n");

    do
    {
        // dump process metadata
        KPrint("Process %x\n", current);
        KPrint("    next   => %x\n", current->listEntry.next);
        KPrint("    prev   => %x\n", current->listEntry.prev);
        KPrint("    cr3    => %x\n", current->cr3);
        KPrint("    entry  => %x\n", current->entry);
        KPrint("    buffer => %x\n\n", current->buffer);

        // iterate
        current = (PROC_HANDLE *)current->listEntry.next;
    } while (current != (PROC_HANDLE *)g_ProcessListHead);

fail:
    return;
}
