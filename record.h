
#ifndef _RECORD_H_
#define _RECORD_H_

#include <stdio.h>

typedef struct process {
    int arrivalTime;
    char* name;
    int serviceTime;
    int memoryRequirement;
} process_t;

process_t* processRead(FILE* f);

void processPrint(process_t* proc);

#endif