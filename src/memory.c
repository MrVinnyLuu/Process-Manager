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

void memoryInit(linkedList_t* memory, int maxMemory) {
    memBlock_t* firstBlock = malloc(sizeof(*firstBlock));
    assert(firstBlock);
    firstBlock->type = 'H';
    firstBlock->start = 0;
    firstBlock->length = maxMemory;
    llistAppend(memory, firstBlock);
}

/* Pop and try to allocate memory to head of input queue.
   If successful (or infinite memory) return the process, else requeue. */
process_t* memoryAssign(int time, linkedList_t* memory, linkedList_t* input) {
    
    process_t* proc = llistPop(input);
    
    // If infinite memory, no need to assign memory
    if (!memory) return proc;

    int assignedAt = memoryAlloc(memory, proc->memoryRequirement);

    if (assignedAt != -1) {
        processReadyPrint(time, proc);
        proc->memoryAssignAt = assignedAt;
        return proc;
    } else {
        llistAppend(input, proc);
        return NULL;
    }

}

int memoryAlloc(linkedList_t* memory, int size) {

    // Find the best-fitting "hole"
    listNode_t* cur = memory->head;
    listNode_t* bestFit = NULL;
    
    while (cur) {

        if (((memBlock_t*)cur->item)->type == 'H'
            && ((memBlock_t*)cur->item)->length >= size) {
            
            // Found a perfect fit
            if (((memBlock_t*)cur->item)->length == size) {
                ((memBlock_t*)cur->item)->type = 'P';
                return ((memBlock_t*)cur->item)->start;
            }

            // Update current best-fit
            if (bestFit == NULL) {
                bestFit = cur;
            } else if (bestFit && ((memBlock_t*)cur->item)->length-size < 
                    ((memBlock_t*)bestFit->item)->length-size) {
                bestFit = cur;
            }

        }

        cur = cur->next;

    }

    // Couldn't find a fit
    if (!bestFit) return -1;

    // Create a new "hole"
    memBlock_t* newBlock = malloc(sizeof(*newBlock));
    assert(newBlock);
    newBlock->type = 'H';
    newBlock->start = ((memBlock_t*)bestFit->item)->start + size;
    newBlock->length = ((memBlock_t*)bestFit->item)->length - size;

    // Insert into the memory list
    listNode_t* newNode = llistNode(newBlock);
    if (bestFit->next != NULL) {
        newNode->next = bestFit->next;
        if (newNode->next) newNode->next->prev = newNode;
    }

    // Allocate the memory block
    ((memBlock_t*)bestFit->item)->type = 'P';
    bestFit->next = newNode;
    newNode->prev = bestFit;
    
    ((memBlock_t*)bestFit->item)->length = size;
    
    return ((memBlock_t*)bestFit->item)->start;

}

void memoryFree(linkedList_t* memory, int assignedAt) {

    // Find the memory block
    listNode_t* cur = memory->head;
    while (cur) {
        if (((memBlock_t*)cur->item)->start == assignedAt) break;
        cur = cur->next;
    }

    // The memory block should always be found
    assert(cur);

    // Change into a hole
    ((memBlock_t*)cur->item)->type = 'H';
    
    // Attempt to merge with next block
    if (cur->next && ((memBlock_t*)cur->next->item)->type == 'H') {
        ((memBlock_t*)cur->item)->length += ((memBlock_t*)cur->next->item)->length;
        listNode_t* temp = cur->next;
        cur->next = cur->next->next;
        if (cur->next) cur->next->prev = cur;
        free(temp->item);
        free(temp);
    }

    // Attempt to merge with previous block
    listNode_t* prev = cur->prev;
    if (prev && ((memBlock_t*)prev->item)->type == 'H') {
        ((memBlock_t*)prev->item)->length += ((memBlock_t*)prev->next->item)->length;
        listNode_t* temp = prev->next;
        prev->next = prev->next->next;
        if (prev->next) prev->next->prev = prev;
        free(temp->item);
        free(temp);
    }   

}

