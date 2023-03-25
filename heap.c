
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "heap.h"

#define INIT_SIZE 4

struct heap {
    void** heap;
    int n;
    int size;
};

heap_t* heapInit(size_t dataSize) {

    heap_t* heap = malloc(sizeof(*heap));
    assert(heap);

    heap->heap = malloc(INIT_SIZE * sizeof(dataSize));
    // heap->heap = malloc(INIT_SIZE * sizeof(*(heap->heap)));
    assert(heap->heap);

    heap->n = 0;
    heap->size = INIT_SIZE;

    heap->heap[0] = -1;

    return heap;

}

void heapPush(heap_t* heap, void* data, int (*getKey)(void*)) {
    
    heap->n++;

    heap->heap[heap->n] = data;

    int i = heap->n;

    while (getKey(heap->heap[i/2]) > getKey(data)) {
        heap->heap[i] = heap->heap[i/2];
        i /= 2;
    }

    heap->heap[i] = data;

    if (heap->n == heap->size) {
        heap->size *= 2;
        heap->heap = realloc(heap->heap, heap->size * sizeof(*(heap->heap)));
        assert(heap->heap);
    }

}

void* heapPop(heap_t* heap, int (*getKey)(void*)) {
    
    void* min = heap->heap[1];
    void* last = heap->heap[heap->n--];
    int cur, child;

    for (cur = 1; cur*2 <= heap->n; cur = child) {

        child = cur*2;

        if (child != heap->n && getKey(heap->heap[child+1]) < getKey(heap->heap[child])) {
            child++;
        }

        if (last > heap->heap[child]) {
            heap->heap[cur] = heap->heap[child];
        } else {
            break;
        }

    }

    heap->heap[cur] = last;
    
    return min;

}

void heapShrink(heap_t* heap) {

    if (heap->size == heap->n) return;

    heap->size = (heap->n > INIT_SIZE) ? heap->n : INIT_SIZE;
    heap->heap = realloc(heap->heap, heap->size * sizeof(*(heap->heap)));
    assert(heap->heap);

}

void heapFree(heap_t* heap) {
    free(heap->heap);
    free(heap);
}