// Inspiration taken from https://www.sanfoundry.com/c-program-implement-heap/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "heap.h"

#define INIT_SIZE 10

heap_t* heapInit() {

    heap_t* heap = malloc(sizeof(*heap));
    assert(heap);

    heap->heap = malloc(INIT_SIZE * sizeof(*(heap->heap)));
    assert(heap->heap);

    heap->n = 0;
    heap->size = INIT_SIZE;

    return heap;

}

void heapPush(heap_t* heap, void* proc, int (compare)(void*,void*)) {

    heap->n++;

    if (heap->n == heap->size) {
        heap->size *= 2;
        heap->heap = realloc(heap->heap, heap->size * sizeof(*(heap->heap)));
        assert(heap->heap);
    }

    heap->heap[heap->n] = proc;

    int i = heap->n;

    while (i/2 > 0 && compare(heap->heap[i/2], proc)) {
        heap->heap[i] = heap->heap[i/2];
        i /= 2;
    }

    heap->heap[i] = proc;

}

void* heapPop(heap_t* heap, int (compare)(void*,void*)) {

    void* min = heap->heap[1];
    
    void* last = heap->heap[heap->n--];
    
    int cur, child;

    for (cur = 1; cur*2 <= heap->n; cur = child) {

        child = cur*2;

        if (child != heap->n && compare(heap->heap[child], heap->heap[child+1])) {
            child++;
        }

        if (compare(last, heap->heap[child])) {
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

// void heapPrint(heap_t* heap) {
//     for (int i = 1; i <= heap->n; i++) {
//         processPrint(heap->heap[i]);
//     }
// }