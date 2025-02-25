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

/* Create our physical frame bitmap */
static uint8_t pmm_bitmap[MAX_PAGE_FRAMES];

/* Initialize page directory and both high/low page tables */
uint32_t kpd[1024] __attribute__((aligned (4096)));
uint32_t kpt_low_fourmb[1024] __attribute__((aligned (4096)));

static void
enable_paging()
{
    // set cr3 to our initial kernel page directory
    __set_cr3((uint32_t)kpd);

    // we can now enable paging by setting cr0
    __asm__ __volatile__ (
        "mov %%cr0,       %%eax  \n" 
        "or  $0x80000000, %%eax  \n"
        "mov %%eax,       %%cr0"
        ::
        : "eax", "memory"
    );
}

/* Identity map virt/phys memory at given page dir index */
static void 
idmap(uint32_t* pt, uint32_t pd_index)
{
    uint32_t i;

    // set each pte a valid paddr (identity mapping)
    for (i = 0; i < PT_NENTRIES; ++i)
    {
        // mark each page as present and offset accordingly
        pt[i] = (pd_index + 1) * (i * PAGE_SIZE) | PAGE_WRITE | PAGE_PRESENT;
    }
}

/* Map a new page table for given page dir entry */
static void
map_pt(uint32_t pt, uint32_t pd_index)
{
    // map page table at index with WP permissions
    kpd[pd_index] = pt | PAGE_WRITE | PAGE_PRESENT;
}

/* Map a physical page to a virtual address */
void
pmm_mappage(uint32_t paddr, uint32_t vaddr)
{
    uint32_t pd_index = 0;
    uint32_t pt_index = 0;
    uint32_t* pt = NULL;
    
    // get page directory and table index from vaddr
    pd_index = vaddr >> 22;
    pt_index = vaddr >> 12 & 0x03FF;

    // get page table from page directory entry
    pt = (uint32_t*)kpd[pd_index];

    // map as WP for now
    pt[pt_index] = paddr | PAGE_WRITE | PAGE_PRESENT;
}

/* Init physical memory manager */
void 
pmm_init(volatile multiboot_info_t* mbd)
{
    kstatus_t status = STATUS_UNKNOWN;

    // zero out or initial page directory
    kmemset(kpd, 0, 1024);
    // zero-out our physical page bitmap 
    kmemset(pmm_bitmap, 0, MAX_PAGE_FRAMES);

    // identity map first 4MB of kernel memory
    idmap(kpt_low_fourmb, 0x0);

    // map initial page table
    map_pt((uint32_t)kpt_low_fourmb, 0x0);

    // finally, enable paging
    enable_paging();
}

