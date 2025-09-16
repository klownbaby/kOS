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

#include "pmm.h"
#include "kernel.h"
#include "kutils.h"
#include "multiboot.h"

/* Create our physical frame bitmap */
static PMM_BITMAP_ENTRY pmmBitmap[MAX_PAGE_FRAMES];

/* Kernel page directory */
__attribute__((aligned (4096))) volatile ULONG g_KernelPageDir[PD_NENTRIES];

/* Identity map first 4MB of address space */
static VOID 
idMap4MB()
{
    ULONG *idPageTable = NULL;
    
    // allocate physical page for new page table
    idPageTable = (ULONG *)PmmAllocNext();

    // set each pte a valid paddr (identity mapping)
    for (ULONG i = 0; i < PT_NENTRIES; ++i)
    {
        // mark each page as present and offset accordingly
        idPageTable[i] = (i * PAGE_SIZE) | PAGE_WRITE | PAGE_PRESENT;
        // mark page as used
        pmmBitmap[i].used = 1;
    }

    // reserve first page in pt mapping for id mapped pages
    g_KernelPageDir[0] = (ULONG)idPageTable | PAGE_WRITE | PAGE_PRESENT;
}

/* Map a new page table for given page dir entry */
static VOID
mapPageTable(ULONG *pd, ULONG pt, ULONG pdIndex)
{
    // map page table at index with WP permissions
    pd[pdIndex] = pt | PAGE_WRITE | PAGE_PRESENT;
}

/* Find first available page frame */
static ULONG
firstFreeFrame(VOID)
{
    PMM_BITMAP_ENTRY entry = { 0 };
    ULONG frame = 0;

    // loop through each bitmap entry to find first unused
    for (ULONG i = 0; i < MAX_PAGE_FRAMES; ++i)
    {
        entry = pmmBitmap[i];

        // check if used, if not then set and break
        if (!entry.used)
        {
            frame = i * PAGE_SIZE;
            break;
        }
    }

    // finally, return our free frame
    return frame;
}

/* Init physical memory manager */
VOID 
PmmInit(VOID)
{
    ULONG size = 0;
    multiboot_memory_map_t* mbEntry = NULL;

    // set our kernel start/end globals on init
    g_KernelStart = (ULONG)&_kernel_start;
    g_KernelEnd = (ULONG)&_kernel_end;

    // zero out or initial page directory and page table
    KMemSet(g_KernelPageDir, 0, 1024);
    // zero-out our physical page bitmap 
    KMemSet(pmmBitmap, 0, MAX_PAGE_FRAMES);

    // now we need to parse our GRUB memory map
    for (ULONG i = 0; i < g_Mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        // get current map entry
        mbEntry = (multiboot_memory_map_t*)(g_Mbd->mmap_addr + i);

        // mark each page frame as used within non-available regions
        if (mbEntry->type != MULTIBOOT_MEMORY_AVAILABLE)
        {
            // allocate range from start of region to end
            KASSERT_PANIC(
                PmmAllocRange(
                    mbEntry->addr_low, 
                    (mbEntry->addr_low + mbEntry->len_low)) != STATUS_SUCCESS, 
                "Failed to initialize physical memory map!\n");
        }
    }

    // id map first 4MB
    idMap4MB();

    // recursive page direcotory mapping
    mapPageTable(g_KernelPageDir, (ULONG)g_KernelPageDir, (PT_VADDR_BASE >> 22));

    // finally, enable paging
    __enablePaging((ULONG)g_KernelPageDir);
}

/* Dump page table and directory mappings */
VOID
PmmDumpPageDir(VOID)
{
    KPrint("Page Directory Mappings:\n");

    for (ULONG i = 0; i < PD_NENTRIES; ++i)
    {
        if (!IS_PRESENT(g_KernelPageDir[i])) continue;
        
        KPrint("    PDE virt[0x%x] phys[0x%x]\n", (i << 22), (g_KernelPageDir[i] & ~(0xFF)));
    }
}

/* Allocate a physcial page in pmmBitmap */
KSTATUS
PmmAllocFrame(ULONG frame)
{
    KSTATUS status = STATUS_UNKNOWN;
    ULONG bitmapIndex = 0;

    // make sure we are page-aligned
    KASSERT_GOTO_FAIL_ERR_MSG(
        !IS_PAGE_ALIGNED(frame), 
        STATUS_FAILED,
        "Frame not aligned on page boundary!\n");

    // get index into pmmBitmap
    bitmapIndex = frame / PAGE_SIZE;

    // make sure our frame isn't already in use
    KASSERT_GOTO_FAIL_ERR_MSG(
        pmmBitmap[bitmapIndex].used, 
        STATUS_IN_USE, 
        "Frame is already allocated!\n");

    // mark frame as allocated
    pmmBitmap[bitmapIndex].used = 1;

    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Allocate all page frames within range */
KSTATUS
PmmAllocRange(ULONG start, ULONG end)
{
    KSTATUS status = STATUS_UNKNOWN;

    // ensure our start and end pointers are page-aligned
    start = PAGE_ALIGN_DOWN(start);
    end = PAGE_ALIGN_UP(end);

    for (ULONG frame = start; frame < end; frame += PAGE_SIZE)
    {
        // allocate each frame
        status = PmmAllocFrame(frame);

        // ensure our allocation succeeded
        KASSERT_GOTO_FAIL_ERR_MSG(
            status != STATUS_SUCCESS, 
            STATUS_FAILED, 
            "Failed to map range!");
    }

    // successfully mapped all page frames
    status = STATUS_SUCCESS;

fail:
    return status;
}

/* Allocate physical frame automatically using next free */
ULONG
PmmAllocNext(VOID)
{
    ULONG frame = 0;

    // get first free page frame
    frame = firstFreeFrame();

    // make sure we can actually allocate the frame
    KASSERT_PANIC(
        PmmAllocFrame(frame) != STATUS_SUCCESS, 
        "First free frame already in use?!");

fail:
    return frame;
}

/* Find physical address mapping for virtual address */
ULONG
PmmVirtToPhys(ULONG *pd, ULONG vaddr)
{
    ULONG pdIndex = 0;
    ULONG ptIndex = 0;
    ULONG* pt = NULL;
    int32_t paddr = -1;

    if (pd == NULL)
    {
        pd = (ULONG *)g_KernelPageDir;
    }
    
    // get page directory and table index from vaddr
    pdIndex = vaddr >> 22;
    ptIndex = vaddr >> 12 & 0x03FF;

    // ensure our page table is present
    KASSERT_GOTO_FAIL_MSG(
        !IS_PRESENT(pd[pdIndex]), "Page table not mapped for vaddr!\n");

    // get virtual address for recursive mapping
    pt = (ULONG *)(PT_VADDR_BASE + (pdIndex * PAGE_SIZE));

    // ensure a frame has been mapped
    KASSERT_GOTO_FAIL_MSG(
        !IS_PRESENT(pt[ptIndex]), "Frame not mapped in page table!\n");

    // ok, we're good
    paddr = pt[ptIndex] & ~(0xF);

fail:
    return paddr;
}

/* Map a physical page to a virtual address */
VOID
PmmMapPage(ULONG *pd, ULONG paddr, ULONG vaddr)
{
    ULONG pdIndex = 0;
    ULONG ptIndex = 0;
    ULONG *pt = NULL;

    if (pd == NULL)
    {
        pd = (ULONG *)g_KernelPageDir;
    }
    
    // get page directory and table index from vaddr
    pdIndex = vaddr >> 22;
    ptIndex = vaddr >> 12 & 0x03FF;

    // should we allocate a new page table
    if (!IS_PRESENT(pd[pdIndex]))
    {
        mapPageTable(pd, PmmAllocNext(), pdIndex);
    }

    // get virtual address for recursive mapping
    pt = (ULONG*)(PT_VADDR_BASE + (pdIndex * PAGE_SIZE));

    // map as WP for now
    pt[ptIndex] = paddr | PAGE_WRITE | PAGE_PRESENT;

    // invalidate vaddr in TLB
    __invlpg(vaddr);
}

MODULE_ENTRY_ORDERED(PmmInit, 3);
