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

/* Find a page table entry */
static void* 
find_pte(pde_t* pgdir, const uint32_t va)
{
    pde_t* pde = NULL;
    pte_t* pte = NULL;

    pde = &pgdir[va];

    return pte;
}

/* Init physical memory manager */
kstatus_t 
pmm_init(volatile multiboot_info_t* mbd)
{
    kstatus_t status = STATUS_UNKNOWN;

    // zero-out our physical page bitmap 
    kmemset(pmm_bitmap, 0, MAX_PAGE_FRAMES);

    status = pm_map_page(0x40000000, 0x7FF70000);
    printk("Successfully mapped page.\n");

    return status;
}

/* Map a physical address to respective PTE */
kstatus_t 
pm_map_page(void* paddr, void* vaddr)
{
    kstatus_t status = STATUS_UNKNOWN;
    uint32_t* pt = NULL;
    uint32_t pde_idx = 0;
    uint32_t pte_idx = 0;

    // get page directory index
    pde_idx = (uint32_t)vaddr >> 22;

    // check if page table is present
    pt = initial_pgdir[pde_idx] & PAGE_PRESENT 
        ? initial_pgdir[pde_idx] : 0x20000;

    // get page table index
    pte_idx = (uint32_t)vaddr >> 12 & 0x03FF;

    // ensure the page is not in use
    KASSERT_GOTO_FAIL_ERR_MSG(
        pt[pte_idx] & PAGE_PRESENT, STATUS_IN_USE, "Page in use!");

    // set physical address at page table index
    pt[pte_idx] = PAGE_ALIGN(paddr) | PAGE_PRESENT;

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Unmap a physical address to respective PTE */
kstatus_t 
pm_umap_page(void* paddr, void* vaddr)
{
    kstatus_t status = STATUS_UNKNOWN;

    return status;
}

/* Print memory regions from multiboot memory map */
void 
pm_display_mm(multiboot_info_t* mbd)
{
    for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t* mbentry = (multiboot_memory_map_t*) (mbd->mmap_addr + i);

        printk("Region -- addr_low=%x addr_high=%x len_low=%x len_high=%x type=%d\n",
               mbentry->addr_low, mbentry->addr_high, mbentry->len_low, mbentry->len_high,
               mbentry->type);
    }
}
