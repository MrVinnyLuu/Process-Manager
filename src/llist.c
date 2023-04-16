/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
llist.c : Implmentation of a doubly linked list 
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include "llist.h"

struct listNode {
    void* item;
    struct listNode* next;
    struct listNode* prev;
};

struct linkedList {
    listNode_t* head;
    listNode_t* tail;
    int n;
};

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

void llistAppend(linkedList_t* llist, listNode_t* node) {

    llist->n++;

    if (llist->n > 1) {

        node->prev = llist->tail;
        llist->tail->next = node;
        llist->tail = node;

    } else {

        llist->head = node;
        llist->tail = node;

    }

}

listNode_t* llistPop(linkedList_t* llist) {

    llist->n--;
    if (llist->n == 0) llist->tail = NULL;

    listNode_t* ret = llist->head;

    llist->head = llist->head->next;

    return ret;

}

void llistFree(linkedList_t* llist) {

    if (llist->n > 0) {
        listNode_t* cur = llist->head;
        while (cur) {   
            listNode_t* tmp = cur;
            cur = cur->next;
            free(tmp->item);
            free(tmp);
        }
    }

    free(llist);
    
}

/*                               Getters/Setters                              */

int llistLen(linkedList_t* llist) {
    return llist->n;
}

listNode_t* llistHead(linkedList_t* llist) {
    return llist->head;
}

void* nodeItem(listNode_t* node) {
    return node->item;
}

listNode_t* nodeNext(listNode_t* node) {
    return node->next;
}

listNode_t* nodePrev(listNode_t* node) {
    return node->prev;
}

void nodeSetNext(listNode_t* node, listNode_t* next) {
    node->next = next;
}

void nodeSetPrev(listNode_t* node, listNode_t* prev) {
    node->prev = prev;
}

