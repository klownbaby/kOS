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

#pragma once

#include <stdint.h>

void
handle_clear(uint32_t argc, char **argv);

void
handle_reboot(uint32_t argc, char **argv);

void
handle_dumpt(uint32_t argc, char **argv);

void
handle_dumpfs(uint32_t argc, char **argv);

void
handle_dumpfl(uint32_t argc, char **argv);

void
handle_neofetch(uint32_t argc, char **argv);

