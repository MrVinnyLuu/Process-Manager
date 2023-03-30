
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "record.h"

typedef struct listNode {
    process_t* proc;
    struct listNode* next;
} listNode_t;

typedef struct linkedList {
    listNode_t* head;
    listNode_t* tail;
    int n;
} linkedList_t;

void queueAdd(linkedList_t* queue, process_t* proc);
process_t* queuePop(linkedList_t* queue);
linkedList_t* queueInit();
void queueFree(linkedList_t* queue);

#endif