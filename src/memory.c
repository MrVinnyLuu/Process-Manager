
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