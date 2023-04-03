
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

    while (cur) {

        if (((memBlock_t*)cur->item)->type == 'H'
            && ((memBlock_t*)cur->item)->length >= size) {
            
            // Perfect fit
            if (((memBlock_t*)cur->item)->length == size) {
                ((memBlock_t*)cur->item)->type = 'H';
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

    // Allocate the memory block
    ((memBlock_t*)bestFit->item)->type = 'P';
    
    memBlock_t* newBlock = malloc(sizeof(*newBlock));
    assert(newBlock);
    newBlock->type = 'H';
    newBlock->start = ((memBlock_t*)bestFit->item)->start + size;
    newBlock->length = ((memBlock_t*)bestFit->item)->length - size;

    listNode_t* newNode = llistNode(newBlock);
    if (bestFit->next != NULL) {
        newNode->next = bestFit->next->next;
        newNode->next->prev = newNode;
    }
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

    if (start == 0) {
        ((memBlock_t*)cur->item)->type = 'H';
        if (cur->next && ((memBlock_t*)cur->next->item)->type == 'H') {
            ((memBlock_t*)cur->item)->length += ((memBlock_t*)cur->next->item)->length;
            listNode_t* temp = cur->next;
            cur->next = cur->next->next;
            free(temp->item);
            free(temp);
        }
        return;
    }

    while (cur) {
        assert(cur->next); ////////////////////////////////////////
        if (((memBlock_t*)cur->next->item)->start == start) {
            break;
        }
        cur = cur->next;
    }

    ((memBlock_t*)cur->next->item)->type = 'H';

    if (cur->next->next && ((memBlock_t*)cur->next->next->item)->type == 'H') {
        ((memBlock_t*)cur->next->item)->length += ((memBlock_t*)cur->next->next->item)->length;
        listNode_t* temp = cur->next->next;
        if (cur->next->next->next) cur->next->next = cur->next->next->next;
        free(temp->item);
        free(temp);
    }

    if (((memBlock_t*)cur->item)->type == 'H') {
        ((memBlock_t*)cur->item)->length += ((memBlock_t*)cur->next->item)->length;
        listNode_t* temp = cur->next;
        if (cur->next->next) cur->next = cur->next->next;
        free(temp->item);
        free(temp);
    }    

}