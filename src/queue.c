
#include "record.h"

typedef struct listNode {
    process_t* proc;
    listNode_t* next;
} listNode_t;

typedef struct linkedList {
    listNode_t* head;
    listNode_t* tail;
    int size;
} linkedList_t;

void queueAdd(linkedList_t* queue, process_t* proc) {
    queue->size++;
    listNode_t* newNode = {proc, NULL};
    if (queue->tail) {
        queue->tail->next = newNode;
        queue->tail = newNode;
    } else {
        queue->head = newNode;
        queue->tail = newNode;
    }
}

process_t* queuePop(linkedList_t* queue) {
    queue->size--;
    listNode_t* retNode = queue->head;
    queue->head = queue->head->next;
    return retNode;
}

