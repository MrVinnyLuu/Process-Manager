
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

int memoryAlloc(linkedList_t* memory, int size) {

    listNode_t* cur = memory->head;
    listNode_t* bestFit = NULL;

    // Find a suitable "hole"
    while (cur) {

        if (((memBlock_t*)cur->item)->type == 'H'
            && ((memBlock_t*)cur->item)->length >= size) {
            
            // Perfect fit
            if (((memBlock_t*)cur->item)->length == size) {
                ((memBlock_t*)cur->item)->type = 'P';
                return ((memBlock_t*)cur->item)->start;
            }

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

void memoryFree(linkedList_t* memory, int start) {
    
    // listNode_t* cur = memory->head;
    // while (cur) {
    //     printf("%c, %d, %d\n",((memBlock_t*)cur->item)->type,((memBlock_t*)cur->item)->start,((memBlock_t*)cur->item)->length);
    //     cur = cur->next;
    // }

    listNode_t* cur = memory->head;

    while (cur) {
        if (((memBlock_t*)cur->item)->start == start) {
            break;
        }
        cur = cur->next;
    }

    assert(cur);

    ((memBlock_t*)cur->item)->type = 'H';
    
    if (cur->next && ((memBlock_t*)cur->next->item)->type == 'H') {
        ((memBlock_t*)cur->item)->length += ((memBlock_t*)cur->next->item)->length;
        listNode_t* temp = cur->next;
        cur->next = cur->next->next;
        if (cur->next) cur->next->prev = cur;
        free(temp->item);
        free(temp);
    }

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