#include "klib.h"
#include "ktypes.h"
#include "kutils.h"
#include "sh.h"
#include "io.h"

/* Process an unknown path input and convert to a full path */
static VOID
buildPath(CHAR *inPath, CHAR *outFull)
{
    ULONG pwdSize = 0;

    // are we opening an absolute path?
    if (inPath[0] == '/')
    {
        // full path is just our input
        KStrCopy(outFull, inPath);

        GOTO_SUCCESS;
    }

    // are we opening a path relative to pwd?
    if (KStrNCmp(inPath, pwd, KSH_PWDBUF_SIZE) != 0)
    {
        pwdSize = KStrLen(pwd);

        KStrCopy(outFull, pwd);

        // if we aren't appending to root pwd, then add a slash
        if (pwd[0] == '/' && pwd[1] != '\0')
        {
            outFull[pwdSize++] = '/';
        }

        // append path to pwd in full buffer
        KStrCopy((CHAR *)(outFull + pwdSize), inPath);
    }
    // otherwise, we are just opening the current pwd
    else
    {
        KStrCopy(outFull, pwd);
    }

success:
    return;
}

/* Handle clear ksh command (clear screen) */
VOID
HandleClear(ULONG argc, CHAR **argv)
{
    TTYClear();
}

/* Handle reboot ksh command (warm reboot) */
VOID
HandleReboot(ULONG argc, CHAR **argv)
{
    __warmReboot();
}

/* Handle dumpt ksh command (dump page tables) */
VOID
HandleDumpt(ULONG argc, CHAR **argv)
{
    PmmDumpPageDir();
}

/* Handle dumpfs ksh command (dump FAT BIOS parameter block) */
VOID
HandleDumpfs(ULONG argc, CHAR **argv)
{
    FatDumpBs();
}

/* Handle dumpfl ksh command (dump free list) */
VOID
HandleDumpfl(ULONG argc, CHAR **argv)
{
    DumpFreeList();
}

/* Handle neofetch ksh command (dumb lol) */
VOID
HandleNeofetch(ULONG argc, CHAR **argv)
{
    TTYNeofetch();
}

/* Poke (read) from a given memory address */
VOID
HandlePoke(ULONG argc, CHAR **argv)
{
    ULONG addr = 0;
    ULONG size = 0;
    UINT8 *buffer = NULL;

    KASSERT_GOTO_FAIL_MSG(argc < 3, "Usage: poke [address] [size]");

    // get address and size from args
    addr = KAToI(argv[1]);
    size = KAToI(argv[2]);

    // for now, restrict size to 32 bytes
    KASSERT_GOTO_FAIL_MSG(size > 0x10, "Inavlid size, must be less than 32!");

    // allocate a scratch buffer
    buffer = KMalloc(size);

    // copy desired size
    KMemCopy(buffer, (VOID *)addr, size);

    // finally, dump
    KPrint("Dumping at address (0x%x)\n", addr);
    KPrint("    { ");

    for (ULONG i = 0; i < size; ++i)
    {
        KPrint("0x%x, ", buffer[i]);
    }

    KPrint("}\n");

fail:
    // ensure we free our scratch buffer
    if (buffer) KFree(buffer);

    return;
}

/* Prod (write) to a given memory address */
VOID
HandleProd(ULONG argc, CHAR **argv)
{
    // do nothing for now
    return;
}

VOID
HandleExec(ULONG argc, CHAR **argv)
{
    KSTATUS status = STATUS_UNKNOWN;
    CHAR fullPath[256] = {0};
    PROC_HANDLE *handle = NULL;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: exec [path]");

    // init process handle
    status = ProcInit(&handle);
    KASSERT_GOTO_FAIL_MSG(
        status != STATUS_SUCCESS, "Failed to init process!");

    // TODO: make this check more robust
    KASSERT_GOTO_FAIL_MSG(
        argv[1][0] == '/' && argv[1][1] == '\0',
        "Not a valid executable!")

    buildPath(argv[1], fullPath);

    // if not, find the requested directory
    handle->buffer = FatOpen(fullPath, &handle->size);
    KASSERT_GOTO_FAIL_MSG(handle->buffer == NULL, "Path not found!");

    // map the process into memory
    status = ProcLoad(handle);
    KASSERT_GOTO_FAIL_MSG(
        status != STATUS_SUCCESS, "Failed to load process!");

    // finally, start the process
    (VOID)ProcExec(handle);

fail:
    if (handle)
    {
        ProcClose(handle);
    }
}

/* Handle `ps` command (list processes) */
VOID
HandlePs(ULONG argc, CHAR **argv)
{
    ProcDumpList();
}

VOID
HandleLs(ULONG argc, CHAR **argv)
{
    CHAR *path = NULL;
    CHAR fullPath[256] = {0};
    VOID *dir = NULL;
    ULONG size = 0;

    // if no args passed, assume pwd
    if (argc < 2)
    {
        path = pwd;
    }
    else
    {
        buildPath(argv[1], fullPath);
        path = fullPath;
    }

    // should we just dump the root directory?
    // no need for a full KStrCmp() call here, just two bytes lol
    if (path[0] == '/' && path[1] == '\0')
    {
        FatDumpRoot();
        GOTO_SUCCESS;
    }

    // if not root, find the requested directory
    dir = FatOpen(path, &size);
    KASSERT_GOTO_FAIL_MSG(dir == NULL, "Path not found!");

    // finally, dump that JAWN
    FatDumpDirectory(dir, size);

fail:
    if (dir)
    {
        KFree(dir);
    }

success:
    return;
}

/* Dump the present working directory */
VOID
HandlePwd(ULONG argc, CHAR **argv)
{
    // just dump pwd
    KPrint("%s\n", pwd);
}

VOID
HandleCat(ULONG argc, CHAR **argv)
{
    CHAR *data = NULL;
    CHAR fullPath[256] = {0};
    ULONG size = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: cat [file name]");

    buildPath(argv[1], fullPath);

    // open a handle (buffer) to our file
    data = FatOpen(fullPath, &size);
    KASSERT_GOTO_FAIL_MSG(data == NULL, "File not found!");

    // just dump as if all data is valid ASCII for now
    for (ULONG i = 0; i < size; ++i)
    {
        KPrint("%c", data[i]);
    }

fail:
    // ensure we free our buffer
    if (data)
    {
        KFree(data);
    }

    return;
}

VOID
HandleCd(ULONG argc, CHAR **argv)
{
    VOID *dir = NULL;
    CHAR fullPath[256] = {0};
    ULONG size = 0;

    KASSERT_GOTO_FAIL_MSG(argc != 2, "Usage: cd [path]");

    if (argv[1][0] == '/' && argv[1][1] == '\0')
    {
        // reset pwd back to root
        KMemSet(pwd, 0, KSH_PWDBUF_SIZE);
        pwd[0] = '/';

        GOTO_SUCCESS;
    }
    else
    {
        buildPath(argv[1], fullPath);

        dir = FatOpen(fullPath, &size);
        KASSERT_GOTO_FAIL_MSG(dir == NULL, "Path not found!");

        // dir was found, update pwd
        KMemSet(pwd, 0, KSH_PWDBUF_SIZE);
        KStrCopy(pwd, fullPath);
    }

fail:
    if (dir)
    {
        KFree(dir);
    }

success:
    return;
}

VOID
HandleGetEnv(ULONG argc, CHAR **argv)
{
    KSTATUS status = STATUS_UNKNOWN;
    VOID *varData = NULL;
    ULONG varSize = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: getenv [name]");

    status = GetEnvVar(argv[1], &varData, &varSize);
    KASSERT_GOTO_FAIL_MSG(status != STATUS_SUCCESS, "Failed to get environment variable");

    KPrint("%s = %s\n", argv[1], varData);
    
fail:
    return;
}

VOID
HandleSetEnv(ULONG argc, CHAR **argv)
{
    KSTATUS status = STATUS_UNKNOWN;
    ULONG inSize = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 3, "Usage: setenv [name] [string]");

    inSize = KStrLen(argv[2]);
    KASSERT_GOTO_FAIL_MSG(inSize == 0, "Data cannot be empty!");

    status = SetEnvVar(argv[1], argv[2], inSize);
    KASSERT_GOTO_FAIL_MSG(status != STATUS_SUCCESS, "Failed to set environment variable");
    
fail:
    return;
}
