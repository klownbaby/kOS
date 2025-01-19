#pragma once

#include <stdint.h>
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "kmalloc.h"
#include "kutils.h"
#include "memory.h"
#include "multiboot.h"
#include "pmm.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"


/* Number of enabled cores */
extern uint32_t g_num_cores;

/* Kernel heap */
extern uint32_t g_heap_start;
extern uint32_t g_heap_end;
