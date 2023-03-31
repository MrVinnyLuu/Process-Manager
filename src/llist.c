
#include <stdlib.h>
#include <assert.h>
#include "llist.h"

linkedList_t* llistInit() {

    linkedList_t* llist = malloc(sizeof(*llist));
    assert(llist);

    llist->head = NULL;
    llist->tail = NULL;

    llist->n = 0;

    return llist;

}

void llistAdd(linkedList_t* llist, void* item) {

    llist->n++;
    listNode_t* newNode = malloc(sizeof(*newNode));
    assert(newNode);
    newNode->item = item;
    if (llist->tail) {
        llist->tail->next = newNode;
        llist->tail = newNode;
    } else {
        llist->head = newNode;
        llist->tail = newNode;
    }

}

void* llistPop(linkedList_t* llist) {

    llist->n--;
    if (llist->n == 0) llist->tail = NULL;

    listNode_t* prevHead = llist->head;
    void* ret = prevHead->item;

    llist->head = llist->head->next;
    
    free(prevHead);

    return ret;

}

void llistFree(linkedList_t* llist) {
    free(llist);
}

