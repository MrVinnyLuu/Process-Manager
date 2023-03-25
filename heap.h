
#ifndef _HEAP_H_
#define _HEAP_H_

typedef struct heap heap_t;

heap_t* heapInit(size_t dataSize);

void heapPush(heap_t* heap, void* data, int (*getKey)(void*));

void* heapPop(heap_t* heap, int (*getKey)(void*));

void heapShrink(heap_t* heap);

void heapFree(heap_t* heap);

#endif