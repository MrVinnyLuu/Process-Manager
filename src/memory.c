
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
    llistAdd(memory, firstBlock);
}

int memoryAlloc(linkedList_t* memory, int size) {

    listNode_t* cur = memory->head;

    while (cur) {
        if (((memBlock_t*)cur->item)->type == 'H'
            && ((memBlock_t*)cur->item)->length > size) {
            break;
        }
        cur = cur->next;
    }

    // Couldn't fit
    if (!cur) return -1;

    ((memBlock_t*)cur->item)->type = 'P';
    if (((memBlock_t*)cur->item)->length != size) {
        memBlock_t* newBlock = malloc(sizeof(*newBlock));
        assert(newBlock);
        newBlock->type = 'H';
        newBlock->start = ((memBlock_t*)cur->item)->start+size;
        newBlock->length = ((memBlock_t*)cur->item)->length-size;
        listNode_t* newNode = malloc(sizeof(*newNode));
        assert(newNode);
        newNode->item = newBlock;
        if (cur->next) newNode->next = cur->next->next;
        cur->next = newNode;
    }
    ((memBlock_t*)cur->item)->length = size;

    return ((memBlock_t*)cur->item)->start;

}

void memoryFree(linkedList_t* memory, int start) {
    
    listNode_t* cur = memory->head;

    if (((memBlock_t*)cur->item)->start == start) {
        ((memBlock_t*)cur->item)->type = 'H';
        if (cur->next && ((memBlock_t*)cur->next->item)->type == 'H') {
            ((memBlock_t*)cur->item)->length += ((memBlock_t*)cur->next->item)->length;
            listNode_t* temp = cur->next;
            if (cur->next->next) cur->next->next = cur->next->next;
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
        if (cur->next->next) cur->next->next = cur->next->next;
        free(temp->item);
        free(temp);
    }    

}