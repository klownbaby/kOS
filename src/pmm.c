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

/* Get initial page directory defined in boot.S */
static page_dir_t root_pgdir[PD_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

/* Print memory regions from multiboot memory map */
void 
display_mm(multiboot_info_t* mbd)
{
    for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t* mbentry = (multiboot_memory_map_t*) (mbd->mmap_addr + i);

        printk("Region -- addr_low=%x addr_high=%x len_low=%x len_high=%x type=%d\n",
               mbentry->addr_low, mbentry->addr_high, mbentry->len_low, mbentry->len_high,
               mbentry->type);
    }
}

void* find_pte(page_dir_t* pgdir, const uint32_t va)
{
  page_dir_t* pde = NULL;
  pte_t* pte = NULL;

  pde = &pgdir[va];

  return pte;
}

/* Init physical memory manager */
kstatus_t 
pmm_init(volatile multiboot_info_t* mbd)
{
    kstatus_t status = STATUS_UNKNOWN;

    // zero-out our page directory
    kmemset(root_pgdir, 0, PD_ENTRIES * sizeof(page_dir_t));

    // set first page directory entry to initial directory
    root_pgdir[0] = initial_page_dir;

    return status;
}

void* 
pm_alloc_frame()
{
    return (void*) 0x0;
}

int 
pm_free()
{
    return 0;
}
