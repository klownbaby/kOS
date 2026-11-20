#include "klib.h"
#include "ktypes.h"
#include "kutils.h"
#include "sh.h"
#include "io.h"

/* Take the relative path and combine it with the pwd */
static VOID *
openFile(CHAR *path, ULONG *size, BOOLEAN setPwd)
{
    CHAR full[256] = {0};
    VOID *data = NULL;
    ULONG pwdSize = 0;

    // are we opening an absolute path?
    if (path[0] == '/')
    {
        data = FatOpen(path, size);
        KASSERT_GOTO_FAIL(data == NULL);

        // discard and update pwd, full path already included
        if (setPwd)
        {
            KMemSet(pwd, 0, KSH_PWDBUF_SIZE);
            KStrCopy(pwd, path);
        }
    }
    // are we opening a path relative to pwd?
    else if (KStrNCmp(path, pwd, KSH_PWDBUF_SIZE) != 0)
    {
        pwdSize = KStrLen(pwd);

        KStrCopy(full, pwd);

        // if we aren't appending to root pwd, then add a slash
        if (pwd[0] == '/' && pwd[1] != '\0')
        {
            full[pwdSize++] = '/';
        }

        // append path to pwd in full buffer
        KStrCopy((CHAR *)(full + pwdSize), path);

        // try to open the file
        data = FatOpen(full, size);
        KASSERT_GOTO_FAIL(data == NULL);

        if (setPwd)
        {
            KMemSet(pwd, 0, KSH_PWDBUF_SIZE);
            KStrCopy(pwd, full);
        }
    }
    // are we just opening the pwd?
    else
    {
        data = FatOpen(pwd, size);
        KASSERT_GOTO_FAIL(data == NULL);
    }

fail:
    /* Caller owns this now!! */
    return data;
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

    // if not, find the requested directory
    handle->buffer = FatOpen(argv[1], &handle->size);
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
    VOID *dir = NULL;
    ULONG size = 0;

    // if no args passed, assume pwd
    if (argc < 2)
    {
        path = pwd;
    }
    else
    {
        path = argv[1];
    }

    // should we just dump the root directory?
    // no need for a full KStrCmp() call here, just two bytes lol
    if (path[0] == '/' && path[1] == '\0')
    {
        FatDumpRoot();
        GOTO_SUCCESS;
    }

    // if not root, find the requested directory
    dir = openFile(path, &size, FALSE);
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
    CHAR *path = NULL;
    BOOLEAN isRelPath = FALSE;
    CHAR *data = NULL;
    ULONG relPathSize = 0;
    ULONG size = 0;

    KASSERT_GOTO_FAIL_MSG(argc < 2, "Usage: cat [file name]");

    if (argv[1][0] != '/')
    {
        // yea, I know it's dangerous, overflow your heart out
        relPathSize = KStrLen(pwd) + KStrLen(argv[1]);

        path = KMalloc(relPathSize);
        KASSERT_GOTO_FAIL_MSG(path == NULL, "Failed to allocate rel path");

        // to ensure we free later
        isRelPath = TRUE;

        // zero that jawn out
        KMemSet(path, 0, relPathSize);

        // combine full and relative path
        KStrCopy(path, pwd);
        KStrCat(path, "/");
        KStrCat(path, argv[1]);
    }
    else
    {
        path = argv[1];
    }

    // open a handle (buffer) to our file
    data = FatOpen(path, &size);
    KASSERT_GOTO_FAIL_MSG(data == NULL, "File not found!");

    // just dump as if all data is valid ASCII for now
    for (ULONG i = 0; i < size; ++i)
    {
        KPrint("%c", data[i]);
    }

fail:
    // path is only on heap if relative
    if (isRelPath)
    {
        KFree(path);
    }

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
        dir = openFile(argv[1], &size, TRUE);
        KASSERT_GOTO_FAIL_MSG(dir == NULL, "Path not found!");
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
