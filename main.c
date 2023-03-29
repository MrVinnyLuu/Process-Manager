#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "record.h"
#include "heap.h"


// ./allocate -f cases/task1/simple.txt -s RR -m best-fit -q 3

void SJF(FILE* f, int q);

int main(int argc, char** argv) {
    
    char* filepath;
    char* scheduler;
    char* memStrat;
    int quantum;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            filepath = strdup(argv[++i]);
            assert(filepath);
        } else if (strcmp(argv[i], "-s") == 0) {
            scheduler = strdup(argv[++i]);
            assert(scheduler);
        } else if (strcmp(argv[i], "-m") == 0) {
            memStrat = strdup(argv[++i]);
            assert(memStrat);
        } else if (strcmp(argv[i], "-q") == 0) {
            quantum = atoi(argv[++i]);
        }
    }

    // printf("file: %s\nscheduler: %s\nmemstrat: %s\nquantum: %d\n", filepath, scheduler, memStrat, quantum);

    FILE* f = fopen(filepath, "r");
    assert(f);

    SJF(f, quantum);   

    fclose(f);

    free(filepath);
    free(scheduler);
    free(memStrat);

    return 0;

}

void SJF(FILE* f, int q) {

    heap_t* heap = heapInit();

    // Set the time to the start quantum of the first process
    process_t* nextProc = processRead(f);
    heapPush(heap, nextProc);
    int curTime = (nextProc->arrivalTime%q == 0) ?
                   nextProc->arrivalTime : (nextProc->arrivalTime/q+1)*q;

    // Add to the ready queue processes that arrive at the same time as the first
    nextProc = processRead(f);
    while (nextProc && nextProc->arrivalTime == curTime) {
        heapPush(heap, nextProc);
        nextProc = processRead(f);
    }

    while (heap->n != 0) {

        // Get the shortest process
        process_t* execProc = heapPop(heap);
        
        // Start the shortest process
        processRunPrint(execProc, curTime, execProc->serviceTime);

        // Calculate the finish time 
        int finTime = curTime + execProc->serviceTime;
        curTime = (finTime%q == 0) ? finTime : (finTime/q+1)*q;

        // Add all the jobs that arrive strictly before the finish time
        if (!nextProc) nextProc = processRead(f);
        while (nextProc && nextProc->arrivalTime <= curTime-q) {
            heapPush(heap, nextProc);
            nextProc = processRead(f);
        }

        // Complete the running process
        processFinPrint(execProc,curTime,heap->n);

        // Add all the jobs that arrive at the same time as the finish time
        if (!nextProc) nextProc = processRead(f);
        while (nextProc && nextProc->arrivalTime <= curTime) {
            heapPush(heap, nextProc);
            nextProc = processRead(f);
        }
    
    }

    heapFree(heap);

}
