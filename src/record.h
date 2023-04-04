/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
record.h : Interface of a "record.c"
------------------------------------------------------------------------------*/

#ifndef _RECORD_H_
#define _RECORD_H_

#include <stdio.h>

typedef struct process {
    int arrivalTime;
    char* name;
    int serviceTime;
    int remainTime;
    int memoryRequirement;
    int memoryAssignAt;
} process_t;

process_t* processRead(FILE* f);

int processCompare(void* proc1, void* proc2);

void processPrint(process_t* proc);

void processRunPrint(int time, process_t* proc);

void processFinPrint(int time, process_t* proc, int procRemaining);

void processReadyPrint(int time, process_t* proc, int assignAt);

void processFree(process_t* proc);

#endif