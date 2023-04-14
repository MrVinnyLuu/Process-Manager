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
    int serviceTime, remainingTime;
    int memoryRequirement, memoryAssignAt;
    int realPID;
    int readInFD, writeOutFD;
} process_t;

process_t* processRead(FILE* f);

int processCompare(void* a, void* b);

void processReadyPrint(int time, process_t* proc);

void processRunPrint(int time, process_t* proc);

void processFinPrint(int time, process_t* proc, int procRemaining, char* hash);

void processFree(process_t* proc);

#endif