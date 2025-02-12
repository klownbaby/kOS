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
#define PD_ENTRIES      1024
#define PD_NOT_PRESENT  0x00000002
#define SUP_RW_PRESENT  0x00000003
#define PAGE_SIZE       4096

/* 4GB of addressable memory */
#define MAX_PAGES       1024 * 1024

/* Common size mappings */
#define KB              1024
#define MB              (1024 * 1024)
#define GB              (1024 * 1024 * 1024)

/* Common sizes */
#define FOURKB          4096

#define PTE_ENTRY(pte, attr) \
    ((uint32_t) pte) | attr
#define ALIGN_UP(size) \
    size + (PG_SIZE - (size % PG_SIZE))

/* Initial page directory defined in boot.S */
extern page_dir_t initial_page_dir;

/* Flush page from TLB */
static inline void __invlpg(uint32_t vaddr)
{
    __asm__ __volatile__ ("invlpg %0" :: "m"(vaddr));
}

kstatus_t pmm_init(volatile multiboot_info_t* mbd);
void display_mm(multiboot_info_t* mbd);
void* pm_alloc_frame();
int pm_free();
