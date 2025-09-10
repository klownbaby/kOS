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

#include <stdint.h>

/* Some common GDT helpers */
#define GDT_ENTRIES 5

#define GDT_HIGH_LIMIT          0xFFFFFFFF
#define GDT_KERNEL_CODE         0x9A
#define GDT_KERNEL_DATA         0x92

#define GDT_USER_CODE           0xFA
#define GDT_USER_DATA           0xF2

#define GDT_TSS                 0x89

#define GDT_KERNEL_GRANULARITY  0xCF
#define GDT_USER_GRANULARITY    0xCF

#define GDT_BASE_LOW(base)      ((base) & 0xFFFF)
#define GDT_BASE_MID(base)      (((base) >> 16) & 0xFF)
#define GDT_BASE_HIGH(base)     (((base) >> 24) & 0xFF)

#define GDT_LIMIT(limit)        ((limit) & 0xFFFF)
#define GDT_FLAGS(limit)        (((limit) >> 16) & 0x0F)
#define GDT_GRANULARITY(gran)   ((gran) & 0xF0)

typedef struct gdt_entry {
    uint16_t    limit;
    uint16_t    base_low;
    uint8_t     base_mid;
    uint8_t     access;
    uint8_t     flags;
    uint8_t     base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdtr {
    uint16_t    limit;
    gdt_entry_t* base;
} __attribute__((packed)) gdtr_t;

typedef struct tss_entry {
	uint32_t prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
	uint32_t esp0;     // The stack pointer to load when changing to kernel mode.
	uint32_t ss0;      // The stack segment to load when changing to kernel mode.
	/* Unused */
	uint32_t esp1;     // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

extern void load_gdt(gdtr_t*);

/* Get our current kernel stack pointer for TSS */
static inline uint32_t __get_kernel_sp()
{
    uint32_t sp = 0;

    __asm__ __volatile__("movl %%esp, %0" : "=r"(sp));

    return sp;
}

/* GDT function defs */
void 
gdt_init();

void
gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

void
tss_set_kernel_sp(uint32_t sp);
