
#ifndef _HEAP_H_
#define _HEAP_H_

#include "record.h"

typedef struct heap {
    process_t** heap;
    int n;
    int size;
} heap_t;

heap_t* heapInit();

void heapPush(heap_t* heap, process_t* proc);

process_t* heapPop(heap_t* heap);

void heapShrink(heap_t* heap);

void heapFree(heap_t* heap);

void heapPrint(heap_t* heap);

#endif