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
#include <stddef.h>
#include <stdbool.h>

#define DECIMAL 10
#define HEX 16
#define BINARY 2


char* 
kitoa(int value, char* str, int base);

uint32_t
katoi(const char *str);

void
kmemset(void* dest, register int data, register size_t length);

void
kmemcpy(void* dest, void* src, register size_t size);

void 
kpanic(char* msg);

void 
kputs(const char* str);

void 
printk(const char* fmt, ...);

void 
sprintk(const char* fmt, char* buffer, ...);
int 
kmemcmp(const void* aptr, const void* bptr, size_t size);