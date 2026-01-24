#include "ktypes.h"
#include "io.h"

#if 0
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLIMPORT extern
#endif

/* Kernel runtime library exports */
DLLIMPORT
VOID KPrint(const CHAR *fmt, ...);

DLLIMPORT
VOID DumpFreeList();

DLLIMPORT
VOID FatDumpBs();

DLLIMPORT
VOID *KMalloc(SIZE size);

DLLIMPORT
VOID KFree(VOID *addr);

DLLIMPORT
VOID *FatOpen(CHAR *path, ULONG *outSize);

DLLIMPORT
VOID FatDumpRoot(VOID);

DLLIMPORT
VOID FatDumpDirectory(VOID *buffer);

DLLIMPORT
CHAR **KStrSplit(CHAR *str, const CHAR delim, ULONG *elemCount);

DLLIMPORT
VOID KMemSet(VOID *dest, register int data, register SIZE length);

DLLIMPORT
ULONG KAToI(const CHAR *str);

DLLIMPORT
ULONG HashStr(CHAR *str);

DLLIMPORT
SIZE KStrLen(const CHAR *str);

DLLIMPORT
VOID TTYPutCRelative(CHAR c, ULONG relx, ULONG rely, BOOLEAN cursor);

DLLIMPORT
VOID TTYPutC(CHAR c);

DLLIMPORT
VOID TTYClear(VOID);

DLLIMPORT
VOID TTYNeofetch(VOID);

DLLIMPORT
VOID TTYWrite(const CHAR* str);

DLLIMPORT
VOID TTYWriteColor(const CHAR* str, VGA_COLOR fg, VGA_COLOR bg);

DLLIMPORT
CHAR KeyboardScanToChar(UINT8 scan);

DLLIMPORT
VOID KeyboardSetInputBuffer(VOID *newBuffer, ULONG *newHead);

DLLIMPORT
UINT8 KGetScan();

DLLIMPORT
VOID KMemCopy(VOID *dest, VOID *src, register SIZE size);

DLLIMPORT
KSTATUS ProcInit(PROC_HANDLE **outHandle);

DLLIMPORT
KSTATUS ProcLoad(PROC_HANDLE *handle);

DLLIMPORT
KSTATUS ProcExec(PROC_HANDLE *handle);

DLLIMPORT
KSTATUS ProcClose(PROC_HANDLE *handle);

DLLIMPORT
VOID ProcDumpList();

DLLIMPORT
VOID PmmDumpPageDir();
