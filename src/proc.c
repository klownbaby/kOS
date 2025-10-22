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
#include "pe.h"
#include "pmm.h"
#include "stdio.h"

/* Load a process */
KSTATUS
ProcLoad(PROC_HANDLE *handle)
{
    KSTATUS status =  STATUS_UNKNOWN;
    ULONG procPage = 0;
    ULONG cr3Phys = 0;
    ULONG *cr3Virt = NULL;
    ULONG sizeOfImage = 0;
    ULONG sizeOfHeaders = 0;
    IMAGE_DOS_HEADER *dosHeader = NULL;
    IMAGE_PE_HEADER *peHeader = NULL;
    IMAGE_SECTION_HEADER *sections = NULL;

    // ensure this bitch is not NULL
    KASSERT_GOTO_FAIL_ERR(handle == NULL, STATUS_INSUFFICIENT_SPACE);

    // size must be aligned on page boundary
    if (!IS_PAGE_ALIGNED(handle->size))
    {
        // align our size up on page boundary
        handle->size = PAGE_ALIGN_UP(handle->size);
    }

    // read PE header section mappings/virtual size of image
    dosHeader = (IMAGE_DOS_HEADER *)handle->buffer;
    peHeader = (IMAGE_PE_HEADER *)((ULONG)dosHeader + (dosHeader->e_lfanew));
    sections = (IMAGE_SECTION_HEADER *)((ULONG)&peHeader->optionalHeader +
                   (ULONG)peHeader->fileHeader.sizeOfOptionalHeader);

    // virtual size of image/headers
    sizeOfImage = peHeader->optionalHeader.sizeOfImage;
    sizeOfHeaders = peHeader->optionalHeader.sizeOfHeaders;

    // get start vaddr of kprocess page directory list
    cr3Virt = (ULONG *)KPROCESS_PDLIST_BASE;

    // allocate and map physical page for new page directory
    cr3Phys = PmmAllocNext();
    KASSERT_GOTO_FAIL_MSG(
        cr3Phys == 0,
        "Failed to allocate physical page for kprocess page directory\n");

    // map kprocess page directory in kernel page directory
    PmmMapPage(g_KernelPageDir, cr3Phys, (ULONG)cr3Virt);

    // map kernel into process address space
    KMemCopy(cr3Virt, g_KernelPageDir, PAGE_SIZE);

    // reset recursive mapping to new page directory
    cr3Virt[PT_VADDR_BASE >> 22] = (ULONG)cr3Phys | PAGE_PRESENT | PAGE_WRITE;

    // set new cr3 temporarily
    __setCr3(cr3Phys);

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

    // set proc entry
    handle->entry = (PROC_ENTRY)((ULONG)KPROCESS_BASE +
                        (ULONG)peHeader->optionalHeader.addressOfEntryPoint);

    // reset cr3 to kernel page directory
    __setCr3((ULONG)g_KernelPageDir);

    // set process handle members
    handle->cr3 = cr3Phys;

    status = STATUS_SUCCESS;

fail:
    return status;
}

KSTATUS
ProcExec(PROC_HANDLE *handle)
{
    KSTATUS status = STATUS_UNKNOWN;
    LONG procRet = 0;

    // switch to per-proccess address space
    __setCr3(handle->cr3);

    // call process entry
    procRet = handle->entry();

    // swap cr3 back to kernel address space
    __setCr3((ULONG)g_KernelPageDir);

    KPrint("Process returned %x\n", procRet);

    return status;
}
