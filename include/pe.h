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

#include "ktypes.h"

typedef struct _IMAGE_FILE_HEADER {
    UINT16 machine;
    UINT16 numberOfSections;
    ULONG  timeDateStamp;
    ULONG  pointerToSymbolTable;
    ULONG  numberOfSymbols;
    UINT16 sizeOfOptionalHeader;
    UINT16 characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    UINT16 virtualAddress;
    UINT16 size;
} IMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_SECTION_HEADER {
  UINT8  name[8];
  union {
    ULONG physicalAddress;
    ULONG virtualSize;
  } misc;
  ULONG  virtualAddress;
  ULONG  sizeOfRawData;
  ULONG  pointerToRawData;
  ULONG  pointerToRelocations;
  ULONG  pointerToLinenumbers;
  UINT16 numberOfRelocations;
  UINT16 numberOfLinenumbers;
  ULONG  characteristics;
} IMAGE_SECTION_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER {
    UINT16 magic;
    UINT8  majorLinkerVersion;
    UINT8  minorLinkerVersion;
    ULONG  sizeOfCode;
    ULONG  sizeOfInitializedData;
    ULONG  sizeOfUninitializedData;
    ULONG  addressOfEntryPoint;
    ULONG  baseOfCode;
    ULONG  baseOfData;
    ULONG  imageBase;
    ULONG  sectionAlignment;
    ULONG  fileAlignment;
    UINT16 majorOperatingSystemVersion;
    UINT16 minorOperatingSystemVersion;
    UINT16 majorImageVersion;
    UINT16 minorImageVersion;
    UINT16 majorSubsystemVersion;
    UINT16 minorSubsystemVersion;
    ULONG  win32VersionValue;
    ULONG  sizeOfImage;
    ULONG  sizeOfHeaders;
    ULONG  checkSum;
    UINT16 subsystem;
    UINT16 dllCharacteristics;
    ULONG  sizeOfStackReserve;
    ULONG  sizeOfStackCommit;
    ULONG  sizeOfHeapReserve;
    ULONG  sizeOfHeapCommit;
    ULONG  loaderFlags;
    ULONG  numberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY dataDirectory[16];
} IMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_PE_HEADER {
    ULONG                   signature;
    IMAGE_FILE_HEADER       fileHeader;
    IMAGE_OPTIONAL_HEADER optionalHeader;
} IMAGE_PE_HEADER;

typedef struct _IMAGE_DOS_HEADER {
    UINT16 e_magic;               
    UINT16 e_cblp;                
    UINT16 e_cp;                  
    UINT16 e_crlc;                
    UINT16 e_cparhdr;             
    UINT16 e_minalloc;            
    UINT16 e_maxalloc;            
    UINT16 e_ss;                  
    UINT16 e_sp;                  
    UINT16 e_csum;                
    UINT16 e_ip;                  
    UINT16 e_cs;                  
    UINT16 e_lfarlc;              
    UINT16 e_ovno;                
    UINT16 e_res[4];              
    UINT16 e_oemid;               
    UINT16 e_oeminfo;             
    UINT16 e_res2[10];            
    LONG   e_lfanew;              
  } IMAGE_DOS_HEADER;
