#include <stdint.h>
#include "kernel.h"

/**
 * Define all Kernel globals here
 */
uint32_t g_num_cores = 0;

/* Kernel start and end globals */
uint32_t g_kernel_start = 0;
uint32_t g_kernel_end = 0;

/* Heap globals */
uint8_t* g_heap_start = (uint8_t*)0x7ff00000;
uint8_t* g_heap_end = 0;
