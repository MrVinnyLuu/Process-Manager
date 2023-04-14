/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
llist.c : Implmentation of a doubly linked list 
------------------------------------------------------------------------------*/

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

listNode_t* llistNode(void* item) {

    listNode_t* newNode = malloc(sizeof(*newNode));
    assert(newNode);

    newNode->item = item;
    newNode->next = NULL;
    newNode->prev = NULL;

    return newNode;

}

void llistAppend(linkedList_t* llist, void* item) {

    assert(item);

    llist->n++;

    listNode_t* newNode = llistNode(item);

    if (llist->head) {

        newNode->prev = llist->tail;
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
    listNode_t* cur = llist->head;
    while (cur) {   
        listNode_t* tmp = cur;
        cur = cur->next;
        free(tmp->item);
        free(tmp);
    }
    free(llist);
}

