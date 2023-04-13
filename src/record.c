/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
record.c : Implmentation of a process record
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "record.h"

#define MAX_NAME_LEN 8

process_t* processRead(FILE* f) {

    process_t* proc = malloc(sizeof(*proc));
    proc->name = malloc(MAX_NAME_LEN);
	assert(proc);

    if (fscanf(f, "%d %s %d %d",
               &proc->arrivalTime,
               proc->name,
               &proc->serviceTime,
               &proc->memoryRequirement) == 4) {

        proc->remainingTime = proc->serviceTime;
        proc->memoryAssignAt = -1;
        proc->realPID = -1;
        proc->writeOutFD = -1;
        proc->readInFD = -1;
        return proc;

    } else {

        processFree(proc);
        return NULL;

    }

}

int processCompare(void* proc1, void* proc2) {

    if (((process_t*)proc1)->serviceTime != ((process_t*)proc2)->serviceTime) {
        return ((process_t*)proc1)->serviceTime > ((process_t*)proc2)->serviceTime;
    } else if (((process_t*)proc1)->arrivalTime != ((process_t*)proc2)->arrivalTime) {
        return ((process_t*)proc1)->arrivalTime > ((process_t*)proc2)->arrivalTime;
    } else {
        return strcmp(((process_t*)proc1)->name, ((process_t*)proc2)->name) > 0;
    }

}

void processReadyPrint(int time, process_t* proc) {
    printf("%d,READY,process_name=%s,assigned_at=%d\n",
            time, proc->name, proc->memoryAssignAt);
}

void processRunPrint(int time, process_t* proc) {
    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n",
            time, proc->name, proc->remainingTime);
}

void processFinPrint(int time, process_t* proc, int procRemaining, char* hash) {
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n",
            time, proc->name, procRemaining);
    printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n",
            time, proc->name, hash);
}

void processFree(process_t* proc) {
    free(proc->name);
    free(proc);
}