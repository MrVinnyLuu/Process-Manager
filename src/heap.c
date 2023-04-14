/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
heap.c : Implementation of a min-heap

Inspiration taken from https://www.sanfoundry.com/c-program-implement-heap/
------------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include "heap.h"

#define INIT_SIZE 10 // Abitrary initial size

heap_t* heapInit() {

    heap_t* heap = malloc(sizeof(*heap));
    assert(heap);

    heap->heap = malloc(INIT_SIZE * sizeof(*(heap->heap)));
    assert(heap->heap);

    heap->n = 0;
    heap->size = INIT_SIZE;

    return heap;

}

void heapPush(heap_t* heap, void* item, int (compare)(void*,void*)) {

    assert(item);

    heap->n++;

    // Ensure there's space in the array
    if (heap->n == heap->size) {
        heap->size *= 2;
        heap->heap = realloc(heap->heap, heap->size * sizeof(*(heap->heap)));
        assert(heap->heap);
    }

    // Insert at end first
    heap->heap[heap->n] = item;

    // "Heapify" until in right spot
    int i = heap->n;
    while (i/2 > 0 && compare(heap->heap[i/2], item)) {
        heap->heap[i] = heap->heap[i/2];
        i /= 2;
    }

    // Insert in the right spot
    heap->heap[i] = item;

}

void* heapPop(heap_t* heap, int (compare)(void*,void*)) {

    // heap[0] is kept empty so minimum item is at [1]
    void* min = heap->heap[1];
    
    void* last = heap->heap[heap->n];
    heap->n--;
    
    int cur, child;

    // Fix the heap
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

void heapFree(heap_t* heap) {
    free(heap->heap);
    free(heap);
}