/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
record.h : Interface of a "record.c"
------------------------------------------------------------------------------*/

#ifndef _RECORD_H_
#define _RECORD_H_

#include <stdio.h>

typedef struct process process_t;

process_t* processRead(FILE* f);

int processCompare(void* a, void* b);

void processIncrement(process_t* proc, int q);

void processAssignMemory(process_t* proc, int assignedAt);

void processSetReal(process_t* proc, int PID, int readInFD, int writeOutFD);

void processReadyPrint(int time, process_t* proc);

void processRunPrint(int time, process_t* proc);

void processFinPrint(int time, process_t* proc, int procRemaining, char* hash);

void processFree(process_t* proc);

/*                                   Getters                                  */

char* processName(process_t* proc);

int processServiceTime(process_t* proc);

int processRemainingTime(process_t* proc);

int processArrivalTime(process_t* proc);

int processMemoryRequirement(process_t* proc);

int processMemoryAssignedAt(process_t* proc);

int processRealPID(process_t* proc);

int processWriteOutFD(process_t* proc);

int processReadInFD(process_t* proc);

#endif