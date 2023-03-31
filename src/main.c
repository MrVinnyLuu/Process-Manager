#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "record.h"
#include "heap.h"
#include "queue.h"

typedef struct stats {
    int turnaround;
    double maxOverhead;
    double avgOverhead;
    int makespan;
} stats_t;

stats_t SJF(FILE* f, int q);
stats_t RR(FILE* f, int q);

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

    // process_t* proc;
    // linkedList_t* queue = queueInit();
    // while ((proc = processRead(f))) {
    //     queueAdd(queue,proc);
    // }
    // while (queue->n != 0) {
    //     processPrint(queuePop(queue));
    // }

    stats_t stats;

    if (strcmp(scheduler, "SJF") == 0) {
        stats = SJF(f, quantum);
    } else if (strcmp(scheduler, "RR") == 0) {
        stats = RR(f, quantum);
    } else {
        return 1;
    }
    
    fprintf(stdout,"Turnaround time %d\n"
                    "Time overhead %.2lf %.2lf\n"
                    "Makespan %d\n",
                    stats.turnaround, roundf(stats.maxOverhead*100)/100,
                    roundf(stats.avgOverhead*100)/100, stats.makespan);

    fclose(f);

    free(filepath);
    free(scheduler);
    free(memStrat);

    return 0;

}

stats_t RR(FILE* f, int q) {

    linkedList_t* queue = queueInit();
    stats_t stats = {};
    int numProcesses = 0, totTurnaround = 0;
    double totOverhead = 0;

    // Set the time to the start quantum of the first process
    process_t* nextProc = processRead(f);
    queueAdd(queue, nextProc);
    int curTime = (nextProc->arrivalTime%q == 0) ?
                   nextProc->arrivalTime : (nextProc->arrivalTime/q+1)*q;

    // Add to the ready queue processes that arrive at the same time as the first
    nextProc = processRead(f);
    while (nextProc && nextProc->arrivalTime == curTime) {
        queueAdd(queue, nextProc);
        nextProc = processRead(f);
    }

    process_t* execProc = NULL;
    process_t* prevProc = NULL;

    while (queue->n > 0) {
        
        // Get the next process
        prevProc = execProc;
        execProc = queuePop(queue);
        if (execProc->serviceTime == execProc->remainTime) numProcesses++;

        // Run the process
        if (prevProc != execProc) processRunPrint(execProc, curTime);
        execProc->remainTime -= q;
        curTime += q;

        // Add all the jobs that have arrived
        while (nextProc && nextProc->arrivalTime <= curTime) {
            queueAdd(queue, nextProc);
            nextProc = processRead(f);
        }

        // Check to see if the process finished in this quantum
        if (execProc->remainTime <= 0) {
            processFinPrint(execProc,curTime,queue->n);
            int curTurnaround = curTime - execProc->arrivalTime;
            totTurnaround += curTurnaround;
            double curOverhead = (double)curTurnaround/execProc->serviceTime;
            totOverhead += curOverhead;
            stats.maxOverhead = fmax(stats.maxOverhead, curOverhead);
            processFree(execProc);
        } else {
            queueAdd(queue, execProc);
        }

        // Skip "gaps" in time
        if (queue->n == 0 && nextProc) {
            queueAdd(queue, nextProc);
            int startTime = curTime + nextProc->arrivalTime;
            curTime = (startTime%q == 0) ? startTime : (startTime/q+1)*q;
            nextProc = processRead(f);
        }

    }

    queueFree(queue);

    stats.turnaround = ceil((double)totTurnaround/numProcesses);
    stats.avgOverhead = totOverhead/numProcesses;
    stats.makespan = curTime;

    return stats;

}

stats_t SJF(FILE* f, int q) {

    heap_t* heap = heapInit();
    stats_t stats = {};
    int numProcesses = 0, totTurnaround = 0;
    double totOverhead = 0;

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

        // heapPrint(heap);

        // Get the shortest process
        process_t* execProc = heapPop(heap);

        // Start the process
        processRunPrint(execProc, curTime);
        numProcesses++;

        // Calculate the finish time 
        int finTime = curTime + execProc->serviceTime;
        curTime = (finTime%q == 0) ? finTime : (finTime/q + 1) * q;

        // Add all the jobs that arrive strictly before the finish time
        while (nextProc && nextProc->arrivalTime <= curTime-q) {
            heapPush(heap, nextProc);
            nextProc = processRead(f);
        }

        // Complete the running process and update stats
        processFinPrint(execProc,curTime,heap->n);
        int curTurnaround = curTime - execProc->arrivalTime;
        totTurnaround += curTurnaround;
        double curOverhead = (double)curTurnaround/execProc->serviceTime;
        totOverhead += curOverhead;
        stats.maxOverhead = fmax(stats.maxOverhead, curOverhead);
        processFree(execProc);

        // Add all the jobs that arrive at the same time as the finish time
        while (nextProc && nextProc->arrivalTime <= curTime) {
            heapPush(heap, nextProc);
            nextProc = processRead(f);
        }

        // Skip "gaps" in time
        if (heap->n == 0 && nextProc) {
            heapPush(heap, nextProc);
            int startTime = curTime + nextProc->arrivalTime;
            curTime = (startTime%q == 0) ? startTime : (startTime/q+1)*q;
            nextProc = processRead(f);
        }

    }

    heapFree(heap);

    stats.turnaround = ceil((double)totTurnaround/numProcesses);
    stats.avgOverhead = totOverhead/numProcesses;
    stats.makespan = curTime;

    return stats;

}
