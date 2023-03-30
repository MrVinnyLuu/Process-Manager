
#ifndef _RECORD_H_
#define _RECORD_H_

#include <stdio.h>

typedef struct process {
    int arrivalTime;
    char* name;
    int serviceTime;
    int remainTime;
    int memoryRequirement;
} process_t;

process_t* processRead(FILE* f);

int processCompare(process_t* proc1, process_t* proc2);

void processPrint(process_t* proc);

void processRunPrint(process_t* proc, int time);

void processFinPrint(process_t* proc, int time, int procRemaining);

void processFree(process_t* proc);

#endif