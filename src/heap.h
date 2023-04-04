/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
heap.h : Interface of "heap.c"
------------------------------------------------------------------------------*/

#ifndef _HEAP_H_
#define _HEAP_H_

typedef struct heap {
    void** heap;
    int n;
    int size;
} heap_t;

heap_t* heapInit();

void heapPush(heap_t* heap, void* proc, int (compare)(void*,void*));

void* heapPop(heap_t* heap, int (compare)(void*,void*));

void heapFree(heap_t* heap);

void heapPrint(heap_t* heap);

#endif