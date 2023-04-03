
#ifndef _LLIST_H_
#define _LLIST_H_

typedef struct listNode {
    void* item;
    struct listNode* next;
    struct listNode* prev;
} listNode_t;

typedef struct linkedList {
    listNode_t* head;
    listNode_t* tail;
    int n;
} linkedList_t;

linkedList_t* llistInit();
listNode_t* llistNode(void* item);
void llistAppend(linkedList_t* llist, void* item);
void* llistPop(linkedList_t* llist);
void llistFree(linkedList_t* llist);

#endif