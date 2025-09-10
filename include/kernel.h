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
#include <stdbool.h>
#include "hash.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "kmalloc.h"
#include "kutils.h"
#include "multiboot.h"
#include "pmm.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"
#include "ksh.h"

/* Module entry point macro for unordered initialization */
#define MODULE_ENTRY(__entry) \
    __attribute__((section(".dynmod"))) module_entry_t *__entry##_dynmod = \
        (module_entry_t *)__entry

/* Module entry point macro for ordered initialization */
#define MODULE_ENTRY_ORDERED(__entry, __n) \
    __attribute__((section(".dynmod.init"#__n))) module_entry_t *__entry##_dynmod = \
        (module_entry_t *)__entry

/* Kernel start/end mappings from linker */
extern volatile uint32_t _kernel_start;
extern volatile uint32_t _kernel_end;

/* Kernel data section from linker */
extern volatile uint32_t _data_start;

extern volatile uint32_t _dynmod_start;
extern volatile uint32_t _dynmod_end;

/* GRUB multiboot info from handoff to kernel */
extern multiboot_info_t *g_mbd;

/* Number of enabled cores */
extern uint32_t g_num_cores;

/* Saving some time for commonly used linker-defined globals */
extern uint32_t g_kernel_start;
extern uint32_t g_kernel_end;

/* Kernel heap */
extern uint8_t *g_heap_start;
extern uint8_t *g_heap_end;
