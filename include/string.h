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

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

size_t 
kstrlen(const char* str);

void 
kstrcpy(char* dest, const char* src);

void 
kstrncpy(char* dest, const char* src, size_t n);

char* 
kstrcat(char* src, const char* append);

bool 
kstrcmp(const char* a, const char* b);

char **
kstrsplit(char *str, const char delim, uint32_t *elem_count);
