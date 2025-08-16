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

#include "kernel.h"
#include "ksh_proc.h"
#include "drivers/tty.h"
#include "drivers/keyboard.h"

/* Our keyboard input buffer */
static char* inputbuf;
/* Initialize head pointer for input buffer */
static uint32_t inputbuf_head = 0;
/* Initialize our command hashmap, making this large (for now) */
static cmd_handler_t cmd_hashmap[0x1000] = { 0 };

/* Our command strings and their associated processors (callbacks) */
static const cmd_handler_t cmd_handlers[5] = {
    { .cmdstr = "clear", .proc = handle_clear },
    { .cmdstr = "reboot", .proc = handle_reboot },
    { .cmdstr = "dumpt", .proc = handle_dumpt },
    // { .cmdstr = "dumpfs", .proc = handle_dumpfs },
    { .cmdstr = "dumpfl", .proc = handle_dumpfl },
    { .cmdstr = "neofetch", .proc = handle_neofetch },
};

/* Build out our initial hashmap for command processors (callbacks) */
static void
build_hashmap()
{
    uint32_t hash = 0;

    for (uint32_t i = 0; i < CMD_LIST_SIZE; ++i)
    {
        // get hash for command string
        hash = hashstr(cmd_handlers[i].cmdstr) % HASHMAP_SIZE;

        // add to hashmap
        cmd_hashmap[hash] = cmd_handlers[i];
    }
}

/* Process data in input buffer as command */
static void
process_cmd()
{
    uint32_t hash = 0;
    uint32_t argc = 0;
    uint32_t elem_size = 0;
    char **argv = NULL;
    char *elem = NULL;
    char *tmp = NULL;

    // ignore any zero-length input buffers, but don't fail
    KASSERT_GOTO_SUCCESS(kstrlen(inputbuf) == 0);

    tmp = inputbuf;

    argc = kstrntok(inputbuf, ' ');
    argv = kmalloc(sizeof(char *) * argc);

    for (uint32_t i = 0; i < argc; ++i)
    {
        elem_size = kstrtokoff(tmp, ' ');

        if (elem_size == 0) break;

        // allocate size + NULL character
        elem = kmalloc(elem_size + 1);
        // zero out buffer
        kmemset(elem, 0, elem_size + 1);

        kmemcpy(elem, tmp, elem_size);

        argv[i] = elem;
        tmp += elem_size;
    }

    // hash our input string
    hash = hashstr(argv[0]) % HASHMAP_SIZE;

    // check that we have a valid command
    if (hash >= HASHMAP_SIZE || cmd_hashmap[hash].cmdstr == NULL)
    {
        printk("Command not found! \"%s\"\n", inputbuf);
        GOTO_FAIL;
    }

    // call our handler
    cmd_hashmap[hash].proc(argc, argv);

fail:
    for (uint32_t i = 0; i < argc; ++i)
    {
        if (argv && argv[i])
        {
            kfree(argv[i]);
        }
    }

    // free argument buffer itself
    kfree(argv);

success:
    // reset input buffer head
    inputbuf_head = 0;

    // reset our input buffer
    kmemset(inputbuf, 0, KSH_INPUTBUF_SIZE);
}

/* Key press notification callback */
static void
kbd_notify_cb(uint8_t scan, uint8_t pressed)
{
    char c;

    // ensure our input buffer is allocated
    KASSERT_GOTO_FAIL_MSG(
        inputbuf == NULL, "inputbuf is not initialized!\n");

    // ignore key up
    KASSERT_GOTO_FAIL(pressed != 0);

    switch (scan)
    {
        case KEY_BACKSPACE:
            // check that we're not at the start of input buffer
            if (inputbuf_head == 0)
            {
                break;
            }

            // remove last char from input buffer
            inputbuf[--inputbuf_head] = 0;

            // remove char from screen
            tty_putc_relative('\0', -1, 0, TRUE);
            break;
        // tilde key reboots
        case KEY_TILDE:
            tty_write("\n");

            // dump freelist
            dump_freelist();

            tty_write("\n");
            tty_writecolor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            break;
        // process keybuf on return
        case KEY_ENTER:
            tty_write("\n");

            // process current command
            process_cmd();

            // write our prompt
            tty_writecolor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            break;
        // otherwise, write ascii
        default:
            // restrict char count
            if (inputbuf_head == (KSH_INPUTBUF_SIZE - 1))
            {
                break;
            }

            // get ascii
            c = keyboard_scan_to_char(scan);

            // set next char in buffer
            inputbuf[inputbuf_head++] = c;

            // finally, display the char
            tty_putc(c);
            break;
    }

fail:
    return;
}

/* Start kernel shell */
void
ksh_init()
{
    // build our command processor hashmap
    build_hashmap();

    // allocate 256 byte input buffer
    inputbuf = (char*)kmalloc(KSH_INPUTBUF_SIZE);
    kmemset(inputbuf, 0, KSH_INPUTBUF_SIZE);

    // set our keypress callback
    keyboard_set_notify_cb(kbd_notify_cb);

    // write initial prompt
    tty_writecolor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
}

void
ksh_fini()
{
    kfree(inputbuf);
}
