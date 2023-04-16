/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
memory.c : Implmentation of simulated memory allocation
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "memory.h"

typedef struct memBlock {
    char type;
    int start;
    int length;
} memBlock_t;

int memoryAlloc(linkedList_t* memory, int size);

void memoryInit(linkedList_t* memory, int maxMemory) {
    memBlock_t* firstBlock = malloc(sizeof(*firstBlock));
    assert(firstBlock);
    firstBlock->type = 'H';
    firstBlock->start = 0;
    firstBlock->length = maxMemory;
    llistAppend(memory, llistNode(firstBlock));
}

/* Pop and try to allocate memory to head of input queue.
   If successful (or infinite memory) return the process, else requeue. */
process_t* memoryAssign(int time, linkedList_t* memory, linkedList_t* input) {
    
    listNode_t* cur = llistPop(input);
    process_t* proc = nodeItem(cur);
    
    // If infinite memory, no need to assign memory
    if (!memory) {
        free(cur);
        return proc;
    }
    
    int assignedAt = memoryAlloc(memory, processMemoryRequirement(proc));

    if (assignedAt != -1) {
        processAssignMemory(proc, assignedAt);
        processReadyPrint(time, proc);
        free(cur);
        return proc;
    } else {
        llistAppend(input, cur);
        return NULL;
    }

}

int memoryAlloc(linkedList_t* memory, int size) {

    // Find the best-fitting "hole"
    listNode_t* cur = llistHead(memory);
    listNode_t* bestFit = NULL;
    
    while (cur) {

        if (((memBlock_t*)nodeItem(cur))->type == 'H'
            && ((memBlock_t*)nodeItem(cur))->length >= size) {
            
            // Found a perfect fit
            if (((memBlock_t*)nodeItem(cur))->length == size) {
                ((memBlock_t*)nodeItem(cur))->type = 'P';
                return ((memBlock_t*)nodeItem(cur))->start;
            }

            // Update current best-fit
            if (!bestFit ||
                ((memBlock_t*)nodeItem(cur))->length-size < 
                ((memBlock_t*)nodeItem(bestFit))->length-size) {

                bestFit = cur;

            }

        }

        cur = nodeNext(cur);

    }

    // Couldn't find a fit
    if (!bestFit) return -1;

    // Create a new "hole"
    memBlock_t* newBlock = malloc(sizeof(*newBlock));
    assert(newBlock);
    newBlock->type = 'H';
    newBlock->start = ((memBlock_t*)nodeItem(bestFit))->start + size;
    newBlock->length = ((memBlock_t*)nodeItem(bestFit))->length - size;

    // Insert into the memory list
    listNode_t* newNode = llistNode(newBlock);
    if (nodeNext(bestFit)) {
        nodeSetNext(newNode, nodeNext(bestFit));
        if (nodeNext(newNode)) nodeSetPrev(nodeNext(newNode), newNode);
    }

    // Allocate the memory block
    ((memBlock_t*)nodeItem(bestFit))->type = 'P';
    nodeSetNext(bestFit, newNode);
    nodeSetPrev(newNode, bestFit);
    
    ((memBlock_t*)nodeItem(bestFit))->length = size;
    
    return ((memBlock_t*)nodeItem(bestFit))->start;

}

void memoryFree(linkedList_t* memory, int assignedAt) {

    // Find the memory block
    listNode_t* cur = llistHead(memory);
    while (cur) {
        if (((memBlock_t*)nodeItem(cur))->start == assignedAt) break;
        cur = nodeNext(cur);
    }

    // The memory block should always be found
    assert(cur);

    // Change into a hole
    ((memBlock_t*)nodeItem(cur))->type = 'H';
    
    // Attempt to merge with next block
    if (nodeNext(cur) && ((memBlock_t*)nodeItem(nodeNext(cur)))->type == 'H') {

        ((memBlock_t*)nodeItem(cur))->length +=
            ((memBlock_t*)nodeItem(nodeNext(cur)))->length;

        listNode_t* temp = nodeNext(cur);

        nodeSetNext(cur, nodeNext(nodeNext(cur)));

        if (nodeNext(cur)) nodeSetPrev(nodeNext(cur), cur);

        free(nodeItem(temp));
        free(temp);

    }

    // Attempt to merge with previous block
    listNode_t* prev = nodePrev(cur);
    if (prev && ((memBlock_t*)nodeItem(prev))->type == 'H') {

        ((memBlock_t*)nodeItem(prev))->length +=
            ((memBlock_t*)nodeItem(cur))->length;

        listNode_t* temp = cur;

        nodeSetNext(prev, nodeNext(nodeNext(prev)));

        if (nodeNext(prev)) nodeSetPrev(nodeNext(prev), prev);

        free(nodeItem(temp));
        free(temp);

    }   

}

