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

/* Kernel start/end mappings from linker */
extern volatile uint32_t _kernel_start;
extern volatile uint32_t _kernel_end;

/* Kernel data section from linker */
extern volatile uint32_t _data_start;

/* Number of enabled cores */
extern uint32_t g_num_cores;

/* Avoiding having to define at compile-time */
extern uint32_t g_kernel_start;
extern uint32_t g_kernel_end;

/* Kernel heap */
extern uint8_t* g_heap_start;
extern uint8_t* g_heap_end;
