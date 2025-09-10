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

/* Virtual address for base of recursive page direcotry mapping */
#define PT_VADDR_BASE   0xFFC00000

/* Align address down to default page size */
#define PAGE_ALIGN_DOWN(__addr) \
  ((uint32_t)(__addr) & ~(0xFFF))

/* Align address up to default page size */
#define PAGE_ALIGN_UP(__addr) \
  ((uint32_t)__addr + (PAGE_SIZE - (__addr & 0xFFF)))

/* Checks that a page aligned on page boundary */
#define IS_PAGE_ALIGNED(__addr) \
  ((__addr & 0xFFF) == 0)

/* Checks that a page is marked present */
#define IS_PRESENT(__addr) \
    ((uint32_t)__addr & PAGE_PRESENT)

/* Flush page from TLB */
static inline void __invlpg(uint32_t vaddr)
{
   asm volatile("invlpg (%0)" ::"r" (vaddr) : "memory"); 
}

/* Get current kernel cr3 register */
static inline uint32_t __get_cr3()
{
    uint32_t cr3 = 0;

    __asm__ __volatile__ ("mov %%cr3, %0" : "=r"(cr3));

    return cr3;
}

/* Set new cr3 value */
static inline void __set_cr3(uint32_t cr3)
{
     __asm__ __volatile__ (
        "mov %0,    %%eax\n"
        "mov %%eax, %%cr3"
        :
        : "r"(cr3)
        : "eax", "memory"
    );
}

static inline void enable_paging(uint32_t pd)
{
    // set cr3 to our initial kernel page directory
    __set_cr3((uint32_t)pd);

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
void
pmm_dumpt(void);

kstatus_t 
pmm_alloc_frame(uint32_t frame);

kstatus_t
pmm_alloc_range(uint32_t start, uint32_t end);

uint32_t
pmm_alloc_next(void);

void 
pmm_map_page(uint32_t paddr, uint32_t vaddr);

int32_t
pmm_virt_to_phys(uint32_t vaddr);

void 
pmm_init(void);

void 
pmm_display_mm(multiboot_info_t* mbd);
