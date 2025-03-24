#pragma once

#include <stdint.h>
#include <stdbool.h>
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
