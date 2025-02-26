#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "file.h"
#include "kmalloc.h"
#include "kutils.h"
#include "multiboot.h"
#include "pmm.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"

/* Kernel start/end mappings from linker */
extern volatile uint32_t _kernel_start;
extern volatile uint32_t _kernel_end;

/* Number of enabled cores */
extern uint32_t g_num_cores;

/* Avoiding having to define at compile-time */
extern uint32_t g_kernel_start;
extern uint32_t g_kernel_end;

/* Kernel heap */
extern uint32_t g_kheap_start;
extern uint32_t g_kheap_end;
