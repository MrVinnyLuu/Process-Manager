
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

    // heap->heap[0] = -1;

    return heap;

}

void heapPush(heap_t* heap, process_t* proc) {
    heap->n++;

    heap->heap[heap->n] = proc;

    int i = heap->n;

    while (i/2 != 0 && heap->heap[i/2]->serviceTime > proc->serviceTime) {
        heap->heap[i] = heap->heap[i/2];
        i /= 2;
    }

    heap->heap[i] = proc;

    if (heap->n == heap->size) {
        heap->size *= 2;
        heap->heap = realloc(heap->heap, heap->size * sizeof(*(heap->heap)));
        assert(heap->heap);
    }

}

process_t* heapPop(heap_t* heap) {
    
    process_t* min = heap->heap[1];
    process_t* last = heap->heap[heap->n--];
    int cur, child;

    for (cur = 1; cur*2 <= heap->n; cur = child) {

        child = cur*2;

        if (child != heap->n && heap->heap[child+1]->serviceTime < heap->heap[child]->serviceTime) {
            child++;
        }

        if (last->serviceTime > heap->heap[child]->serviceTime) {
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