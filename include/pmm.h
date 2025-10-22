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

#include "multiboot.h"
#include "ktypes.h"

/* Page-specific */
#define PD_NENTRIES     1024
#define PT_NENTRIES     1024
#define PAGE_SIZE       4096
#define ID_MAP_END      4 * MB

/* Page acccess masks */
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITE      (1 << 1)

/* 4GB of addressable memory */
#define MAX_PAGE_FRAMES 1024 * 1024

/* Common size mappings */
#define KB              1024
#define MB              (1024 * 1024)
#define GB              (1024 * 1024 * 1024)

/* Common sizes */
#define FOURKB          4 * KB
#define ONEMB           1 * MB

/* Virtual address for base of recursive page directory mapping */
#define PT_VADDR_BASE   0xFFC00000

/* Align address down to default page size */
#define PAGE_ALIGN_DOWN(__addr) \
  ((ULONG)(__addr) & ~(0xFFF))

/* Align address up to default page size */
#define PAGE_ALIGN_UP(__addr) \
  ((ULONG)__addr + (PAGE_SIZE - (__addr & 0xFFF)))

/* Checks that a page aligned on page boundary */
#define IS_PAGE_ALIGNED(__addr) \
  ((__addr & 0xFFF) == 0)

/* Checks that a page is marked present */
#define IS_PRESENT(__addr) \
    ((ULONG)__addr & PAGE_PRESENT)

/* Flush page from TLB */
inline VOID __invlpg(ULONG vaddr)
{
   asm volatile("invlpg (%0)" ::"r" (vaddr) : "memory"); 
}

/* Get current kernel cr3 register */
inline ULONG __getCr3(VOID)
{
    ULONG cr3 = 0;

    __asm__ __volatile__ ("mov %%cr3, %0" : "=r"(cr3));

    return cr3;
}

/* Set new cr3 value */
inline VOID __setCr3(ULONG cr3)
{
     __asm__ __volatile__ (
        "mov %0,    %%eax\n"
        "mov %%eax, %%cr3"
        :
        : "r"(cr3)
        : "eax", "memory"
    );
}

/* Enable paging */
inline VOID __enablePaging(ULONG pd)
{
    // set cr3 to our initial kernel page directory
    __setCr3((ULONG)pd);

    // we can now enable paging by setting cr0
    __asm__ __volatile__ (
        "mov %%cr0,       %%eax  \n" 
        "or  $0x80000000, %%eax  \n"
        "mov %%eax,       %%cr0"
        ::
        : "eax", "memory"
    );
}

/* PMM function definitions */
VOID 
PmmInit(VOID);

VOID
PmmDumpPageDir(VOID);

KSTATUS 
PmmAllocFrame(ULONG frame);

KSTATUS
PmmAllocRange(ULONG start, ULONG end);

ULONG
PmmAllocNext(VOID);

VOID 
PmmMapPage(ULONG *pd, ULONG paddr, ULONG vaddr);

ULONG
PmmVirtToPhys(ULONG *pd, ULONG vaddr);
