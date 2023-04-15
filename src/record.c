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

/* Return <= 0 if process "a" comes before process "b" according to the spec */
int processCompare(void* a, void* b) {

    assert(a && b);
    process_t* proc1 = ((process_t*)a);
    process_t* proc2 = ((process_t*)b);

    if (proc1->serviceTime != proc2->serviceTime) {
        return proc1->serviceTime > proc2->serviceTime;
    } else if (proc1->arrivalTime != proc2->arrivalTime) {
        return proc1->arrivalTime > proc2->arrivalTime;
    } else {
        return strcmp(proc1->name, proc2->name) > 0;
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