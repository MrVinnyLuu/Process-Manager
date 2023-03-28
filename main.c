#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "record.h"
#include "heap.h"


// ./allocate -f cases/task1/simple.txt -s RR -m best-fit -q 3

int main(int argc, char** argv) {
    
    char* filepath;
    char* scheduler;
    char* memStrat;
    char* quantum;

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
            quantum = strdup(argv[++i]);
            assert(quantum);
        }
    }

    // printf("file: %s\nscheduler: %s\nmemstrat: %s\nquantum: %s\n", filepath, scheduler, memStrat, quantum);

    FILE* f = fopen(filepath, "r");
    assert(f);

    heap_t* heap = heapInit();

    process_t* proc = processRead(f);
    process_t* curProc;

    int curTime = proc->arrivalTime;

    heapPush(heap, proc);

    while ((proc = processRead(f))) {

        if (proc->arrivalTime > curTime) {
            
            if (heap->n != 0) {
                curProc = heapPop(heap);
                processRunPrint(curProc, curTime, curProc->serviceTime);
                curTime += curProc->serviceTime;
            } else {
                curTime = proc->arrivalTime;
            }
        }

        processFinPrint(curProc,curTime,heap->n);
        heapPush(heap, proc);
    }

    while (heap->n != 0) {
        process_t* curProc = heapPop(heap);
        processRunPrint(curProc, curTime, curProc->serviceTime);
        curTime += curProc->serviceTime;
        processFinPrint(curProc,curTime,heap->n);
    }

    // for (int i = 1; i <= heap->n; i++) {
    //     processPrint(heap->heap[i]);
    // }

    // while (heap->n > 0) {
    //     processPrint(heapPop(heap));
    // }

    

    fclose(f);

    free(filepath);
    free(scheduler);
    free(memStrat);
    free(quantum);

    heapFree(heap);

    return 0;

}
