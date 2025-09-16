#include <stdint.h>
#include "kernel.h"

/**
 * Define all Kernel globals here
 */

 /* Number of enabled cores */
ULONG g_NumCores = 0;

/* GRUB multiboot info from handoff to kernel */
MULTIBOOT_INFO *g_Mbd = NULL;

/* Global console TTY state */
TTY_STATE g_TTYState = { 0 };

/* Kernel start and end globals */
ULONG g_KernelStart = 0;
ULONG g_KernelEnd = 0;

/* Heap globals */
UINT8 *g_HeapStart = (UINT8 *)0x7ff00000;
UINT8 *g_HeapEnd = 0;
