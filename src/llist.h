/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
llist.h : Interface of "llist.c" 
------------------------------------------------------------------------------*/

#ifndef _LLIST_H_
#define _LLIST_H_

typedef struct listNode listNode_t;

typedef struct linkedList linkedList_t;

linkedList_t* llistInit();

listNode_t* llistNode(void* item);

void llistAppend(linkedList_t* llist, listNode_t* node);

listNode_t* llistPop(linkedList_t* llist);

void llistFree(linkedList_t* llist);

/*                               Getters/Setters                              */

int llistLen(linkedList_t* llist);

listNode_t* llistHead(linkedList_t* llist);

void* nodeItem(listNode_t* node);

listNode_t* nodeNext(listNode_t* node);

listNode_t* nodePrev(listNode_t* node);

void nodeSetNext(listNode_t* node, listNode_t* next);

void nodeSetPrev(listNode_t* node, listNode_t* prev);

#endif