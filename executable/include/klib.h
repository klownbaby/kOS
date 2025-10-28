#include "ktypes.h"

__declspec(dllimport)
VOID KPrint(const CHAR *fmt, ...);

__declspec(dllimport)
VOID DumpFreeList();

__declspec(dllimport)
VOID FatDumpBs();

__declspec(dllimport)
VOID *KMalloc(SIZE size);

__declspec(dllimport)
VOID KFree(VOID *addr);

__declspec(dllimport)
VOID *FatOpen(CHAR *path, ULONG *outSize);
