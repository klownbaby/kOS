#include "klib.h"
#include "ktypes.h"
#include "kutils.h"
#include "sh.h"

/* Our keyboard input buffer */
static CHAR *inputBuffer = NULL;

/* Initialize head pointer for input buffer */
static ULONG inputBufferHead = 0;

/* Initialize our command hashmap, making this large (for now) */
static CMD_HANDLER cmdHashmap[0x1000] = { 0 };

/* Our command strings and their associated processors (callbacks) */
static const CMD_HANDLER cmdHandlers[] = {
    { .cmdstr = "clear", .proc = HandleClear },
    { .cmdstr = "reboot", .proc = HandleReboot },
    { .cmdstr = "dumpt", .proc = HandleDumpt },
    { .cmdstr = "dumpfs", .proc = HandleDumpfs },
    { .cmdstr = "dumpfl", .proc = HandleDumpfl },
    { .cmdstr = "neofetch", .proc = HandleNeofetch },
    { .cmdstr = "poke", .proc = HandlePoke },
    { .cmdstr = "prod", .proc = HandleProd },
    { .cmdstr = "exec", .proc = HandleExec },
    { .cmdstr = "ls", .proc = HandleLs },
    { .cmdstr = "cat", .proc = HandleCat },
    { .cmdstr = "ps", .proc = HandlePs },
    { .cmdstr = "getenv", .proc =  HandleGetEnv },
    { .cmdstr = "setenv", .proc = HandleSetEnv }
};

/* Build out our initial hashmap for command processors (callbacks) */
static VOID
buildHashMap(VOID)
{
    ULONG hash = 0;

    for (ULONG i = 0; i < CMD_LIST_SIZE; ++i)
    {
        // get hash for command string
        hash = HashStr(cmdHandlers[i].cmdstr) % HASHMAP_SIZE;

        // add to hashmap
        cmdHashmap[hash] = cmdHandlers[i];
    }
}

/* Process data in input buffer as command */
static VOID
processCommand(VOID)
{
    ULONG hash = 0;
    ULONG argc = 0;
    ULONG elemSize = 0;
    CHAR **argv = NULL;
    CHAR *elem = NULL;
    CHAR *tmp = NULL;

    // ignore any zero-length input buffers, but don't fail
    KASSERT_GOTO_SUCCESS(KStrLen(inputBuffer) == 0);

    tmp = inputBuffer;

    // split and count arguments
    argv = KStrSplit(tmp, ' ', &argc);

    // check that our arguments were allocated properly
    KASSERT_GOTO_FAIL_MSG(argv == NULL, "Input buffer corrupted!");

    // hash our input string
    hash = HashStr(argv[0]) % HASHMAP_SIZE;

    // check that we have a valid command
    if (hash >= HASHMAP_SIZE || cmdHashmap[hash].cmdstr == NULL)
    {
        KPrint("Command not found! \"%s\"\n", inputBuffer);
        GOTO_FAIL;
    }

    // call our handler
    cmdHashmap[hash].proc(argc, argv);

fail:
    if (argv)
    {
        // free each sub string
        for (ULONG i = 0; i < argc; ++i)
        {
            if (argv[i]) KFree(argv[i]);
        }

        // free argument buffer itself
        KFree(argv);
    }

success:
    // reset input buffer head
    inputBufferHead = 0;

    // reset our input buffer
    KMemSet(inputBuffer, 0, KSH_INPUTBUF_SIZE);
}

/* Key press handler */
static VOID
handleKeyPress(UINT8 scan)
{
    CHAR c;

    // ensure our input buffer is allocated
    KASSERT_GOTO_FAIL_MSG(
        inputBuffer == NULL, "inputBuffer is not initialized!\n");
    
    switch (scan)
    {
        case KEY_BACKSPACE:
            // check that we're not at the start of input buffer
            if (inputBufferHead == 0)
            {
                break;
            }

            // remove last CHAR from input buffer
            inputBuffer[--inputBufferHead] = 0;

            // remove CHAR from screen
            TTYPutCRelative('\0', -1, 0, TRUE);
            break;
        case KEY_TILDE:
            TTYWrite("\n");

            // dump freelist
            DumpFreeList();

            TTYWrite("\n");
            TTYWriteColor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            break;
        case KEY_ENTER:
            TTYWrite("\n");

            // process current command
            processCommand();

            // write our prompt
            TTYWriteColor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            break;
        default:
            // restrict CHAR count
            if (inputBufferHead == (KSH_INPUTBUF_SIZE - 1))
            {
                break;
            }

            // get ascii
            c = KeyboardScanToChar(scan);

            // set next CHAR in buffer
            inputBuffer[inputBufferHead++] = c;

            // finally, display the CHAR
            TTYPutC(c);
            break;
    }

fail:
    return;
}

ULONG
_main()
{
    UINT8 scan = 0;
    ULONG size = 0;

    // build our command processor hashmap
    buildHashMap();

    // init input buffer
    inputBuffer = KMalloc(KSH_INPUTBUF_SIZE);
    KMemSet(inputBuffer, 0, KSH_INPUTBUF_SIZE);

    // write initial prompt
    TTYWriteColor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);

    for (;;)
    {
        // get next scan code
        scan = KGetScan();

        if (scan != 0)
        {
            handleKeyPress(scan);
        }
    }

    // clean up
    KFree(inputBuffer);

    return 0;
}
