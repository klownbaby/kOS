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

/* Initial page table */
uint32_t g_initial_pt[1024] __attribute__((aligned (4096)));

/* Walk a page directory until pde is found */
static uint32_t*
walkpd(uint32_t* cr3, void* vaddr)
{
    uint32_t* pd = cr3;
    uint32_t pde_idx = 0;

    // get pde index from vaddr offset
    pde_idx = (uint32_t)vaddr >> 22;

    return pd[pde_idx];
}

/* Identity map the first 1MB */
static void
pg_idmap()
{
    uint32_t id_map_low = 0x0;
    uint32_t id_map_high = ONEMB / PAGE_SIZE;
    uint32_t i;

    // identity map lowest 1MB
    for (i = 0; i < id_map_high; ++i)
    {
        // set permissions and present
        g_initial_pt[i] = (i * PAGE_SIZE) | PAGE_WRITE | PAGE_PRESENT;
    }
}

/* Initialize paging directory and tables */
static void
pg_init()
{
    uint32_t i;

    // zero-out initial page table
    kmemset(g_initial_pt, 0, PT_NENTRIES);

    // id map the first 1MB
    pg_idmap();

    // set first page table in page directory
    g_initial_pd[1] = (uint32_t)g_initial_pt | PAGE_WRITE | PAGE_PRESENT;
}

/* translate a virtual address to it's physical mapping */
uint32_t*
pm_get_paddr(uint32_t* cr3, void* vaddr)
{
    uint32_t* pde = NULL;
    uint32_t* pt = NULL;
    uint32_t pte = 0;
    uint32_t pte_idx = 0;

    // walk the page directory until pt is found
    pt = walkpd(cr3, vaddr);
    KASSERT_GOTO_FAIL(!IS_PRESENT(pde));

    // get pt index from vaddr offset
    pte_idx = (uint32_t)vaddr >> 12 & 0x03FF;

    // get pte from pt and index
    pte = IS_PRESENT(pt[pte_idx]) ? pt[pte_idx] : 0;

fail:
    // will return 0 if not present
    return pte;
}

/* Init physical memory manager */
kstatus_t 
pmm_init(volatile multiboot_info_t* mbd)
{
    kstatus_t status = STATUS_UNKNOWN;
    uint32_t* cr3 = g_initial_pd;

    // zero-out our physical page bitmap 
    kmemset(pmm_bitmap, 0, MAX_PAGE_FRAMES);

    // init and identity map
    pg_init();

    printk("cr3 -> %x\n", cr3);

    printk("Bad paddr -> %x\n", pm_get_paddr(cr3, 0x9AAA0000));
    printk("Mapped paddr -> %x\n", pm_get_paddr(cr3, 0xc0000000));

fail:
    return status;
}

/* Map a physical address to respective PTE */
kstatus_t 
pm_map_page(uint32_t* cr3, void* paddr, void* vaddr)
{
    kstatus_t status = STATUS_UNKNOWN;
    uint32_t* pd = cr3;
    uint32_t* pt = NULL;
    uint32_t pde_idx = 0;
    uint32_t pte_idx = 0;

    // get page directory index
    pde_idx = (uint32_t)vaddr >> 22;

    if (IS_PRESENT(pd[pde_idx]))
    {
        pt = pd[pde_idx];
    } else {
        pt = pd[pde_idx] = 0x200000 | PAGE_PRESENT | PAGE_WRITE;
    }

    // get page table index
    pte_idx = (uint32_t)vaddr >> 12 & 0x03FF;

    // ensure the page is not in use
    KASSERT_GOTO_FAIL_ERR_MSG(
        IS_PRESENT(pt[pte_idx]), STATUS_IN_USE, "Page in use!");

    // set physical address at page table index
    pt[pte_idx] = PAGE_ALIGN(paddr) | PAGE_PRESENT;

    // flush TLB
    __invlpg((uint32_t)vaddr);

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Unmap a physical address to respective PTE */
kstatus_t 
pm_umap_page(uint32_t* cr3, void* paddr, void* vaddr)
{
    kstatus_t status = STATUS_UNKNOWN;
    uint32_t* pt = NULL;
    uint32_t pde_idx = 0;
    uint32_t pte_idx = 0;

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
