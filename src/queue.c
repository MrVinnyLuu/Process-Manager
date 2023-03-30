
#include <stdlib.h>
#include <assert.h>
#include "queue.h"

linkedList_t* queueInit() {

    linkedList_t* queue = malloc(sizeof(*queue));
    assert(queue);

    queue->n = 0;

    return queue;

}

void queueAdd(linkedList_t* queue, process_t* proc) {

    queue->n++;
    listNode_t* newNode = malloc(sizeof(*newNode));
    assert(newNode);
    newNode->proc = proc;
    if (queue->tail) {
        queue->tail->next = newNode;
        queue->tail = newNode;
    } else {
        queue->head = newNode;
        queue->tail = newNode;
    }

}

process_t* queuePop(linkedList_t* queue) {
    queue->n--;
    if (queue->n == 0) queue->tail = NULL;
    listNode_t* retNode = queue->head;
    queue->head = queue->head->next;
    return retNode->proc;
}

void queueFree(linkedList_t* queue) {
    free(queue);
}

