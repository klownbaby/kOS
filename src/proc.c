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

/* Load a process */
KSTATUS
ProcLoad(PROC_HANDLE *handle)
{
    KSTATUS status =  STATUS_UNKNOWN;
    ULONG procPage = 0;
    ULONG cr3Phys = 0;
    ULONG *cr3Virt = NULL;

    // ensure this bitch is not NULL
    KASSERT_GOTO_FAIL_ERR(handle == NULL, STATUS_INSUFFICIENT_SPACE);

    // size must be aligned on page boundary
    if (!IS_PAGE_ALIGNED(handle->size))
    {
        // align our size up on page boundary
        handle->size = PAGE_ALIGN_UP(handle->size);
    }

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

    for (ULONG i = 0; i < handle->size; i += PAGE_SIZE)
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

    // copy process data to newly mapped pages
    KMemCopy((VOID *)KPROCESS_BASE, handle->buffer, handle->size);

    // set process handle members
    handle->cr3 = cr3Phys;
    handle->entry = (PROC_ENTRY)KPROCESS_BASE;

    status = STATUS_SUCCESS;

fail:
    return status;
}

KSTATUS
ProcExec(PROC_HANDLE *handle)
{
    KSTATUS status = STATUS_UNKNOWN;
    LONG procRet = 0;

    // each process has it's own address space
    __setCr3(handle->cr3);

    // call process entry
    procRet = handle->entry();

    // swap cr3 back to kernel page directory
    __setCr3((ULONG)g_KernelPageDir);

    KPrint("Process returned %x\n", procRet);

    return status;
}
