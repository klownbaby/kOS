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

#include "pmm.h"
#include "kernel.h"
#include "kutils.h"
#include "multiboot.h"

/* Create our physical frame bitmap */
static pmm_bitmap_entry_t pmm_bitmap[MAX_PAGE_FRAMES];

/* Initialize page directory and both high/low page tables */
static volatile uint32_t kpd[PD_NENTRIES] __attribute__((aligned (4096)));
static volatile uint32_t kpt_low_fourmb[PT_NENTRIES] __attribute__((aligned (4096)));

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

/* Allocate a physcial page in pmm_bitmap */
kstatus_t
pmm_alloc_frame(uint32_t frame)
{
    kstatus_t status = STATUS_UNKNOWN;
    uint32_t bitmap_index = 0;

    // make sure we are page-aligned
    KASSERT_GOTO_FAIL_ERR_MSG(
        !IS_PAGE_ALIGNED(frame), 
        STATUS_FAILED,
        "Frame not aligned on page boundary!\n");

    // get index into pmm_bitmap
    bitmap_index = frame / PAGE_SIZE;

    // make sure our frame isn't already in use
    KASSERT_GOTO_FAIL_ERR_MSG(
        pmm_bitmap[bitmap_index].used, 
        STATUS_IN_USE, 
        "Frame is already allocated!\n");

    // mark frame as allocated
    pmm_bitmap[bitmap_index].used = 1;

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Allocate all page frames within range */
kstatus_t
pmm_alloc_range(uint32_t start, uint32_t end)
{
    kstatus_t status = STATUS_UNKNOWN;

    // ensure our start and end pointers are page-aligned
    start = PAGE_ALIGN_DOWN(start);
    end = PAGE_ALIGN_UP(end);

    for (uint32_t frame = start; frame < end; frame += PAGE_SIZE)
    {
        // allocate each frame
        status = pmm_alloc_frame(frame);

        // ensure our allocation succeeded
        KASSERT_GOTO_FAIL_ERR_MSG(
            status != STATUS_SUCCESS, 
            STATUS_FAILED, 
            "Failed to map range!");
    }

    // successfully mapped all page frames
    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Map a physical page to a virtual address */
void
pmm_map_page(uint32_t paddr, uint32_t vaddr)
{
    uint32_t pd_index = 0;
    uint32_t pt_index = 0;
    uint32_t* pt = NULL;
    
    // get page directory and table index from vaddr
    pd_index = vaddr >> 22;
    pt_index = vaddr >> 12 & 0x03FF;

    // invalidate vaddr in TLB
    __invlpg(vaddr);

    // get page table from page directory entry
    pt = (uint32_t*)kpd[pd_index];

    // map as WP for now
    pt[pt_index] = paddr | PAGE_WRITE | PAGE_PRESENT;
}

/* Init physical memory manager */
void 
pmm_init(volatile multiboot_info_t* mbd)
{
    uint32_t mem_high = 0;
    uint32_t phys_alloc_start = 0;
    uint32_t size = 0;
    multiboot_memory_map_t* mbentry = NULL;

    /* Set our kernel start/end globals on init */
    g_kernel_start = (uint32_t)&_kernel_start;
    g_kernel_end = (uint32_t)&_kernel_end;

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

    mem_high = mbd->mem_upper;
    phys_alloc_start = (mem_high + 0xFFF) & ~0xFFF;

    // now we need to parse our GRUB memory map
    for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        mbentry = (multiboot_memory_map_t*)(mbd->mmap_addr + i);

        // mark each page frame as used within non-available regions
        if (mbentry->type != MULTIBOOT_MEMORY_AVAILABLE)
        {
            // allocate range from start of region to end
            KASSERT_PANIC(
                pmm_alloc_range(
                    mbentry->addr_low, 
                    (mbentry->addr_low + mbentry->len_low)) != STATUS_SUCCESS, 
                "Failed to initialize physical memory map!");
        }
    }
}

