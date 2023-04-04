/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
memory.h : Interface of "memory.c"
------------------------------------------------------------------------------*/

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "llist.h"

typedef struct memBlock {
    char type;
    int start;
    int length;
} memBlock_t;

void memoryInit(linkedList_t* memory, int maxMemory);

int memoryAlloc(linkedList_t* memory, int size);

void memoryFree(linkedList_t* memory, int start);

#endif

