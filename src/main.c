#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "record.h"
#include "heap.h"
#include "llist.h"
#include "memory.h"

#define MAX_MEMORY 2048

typedef struct stats {
    int turnaround;
    double maxOverhead;
    double avgOverhead;
    int makespan;
} stats_t;

stats_t SJF(FILE* f, int q, char* memStrat);
stats_t RR(FILE* f, int q, char* memStrat);

int main(int argc, char** argv) {
    
    // Default settings
    char* filepath = "cases/task1/simple.txt";
    char* scheduler = "SJF";
    char* memStrat = "infinite";
    int quantum = 1;

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

    stats_t stats;

    if (strcmp(scheduler, "SJF") == 0) {
        stats = SJF(f, quantum, memStrat);
    } else if (strcmp(scheduler, "RR") == 0) {
        stats = RR(f, quantum, memStrat);
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

stats_t RR(FILE* f, int q, char* memStrat) {

    linkedList_t* memory = NULL;
    linkedList_t* waiting = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        waiting = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }

    linkedList_t* queue = llistInit();
    stats_t stats = {};
    int numProcesses = 0, totTurnaround = 0;
    double totOverhead = 0;

    // Set the time to the start quantum of the first process
    process_t* nextProc = processRead(f);
    llistAppend(queue, nextProc);
    int curTime = (nextProc->arrivalTime%q == 0) ?
                   nextProc->arrivalTime : (nextProc->arrivalTime/q+1)*q;

    if (memory) {
        int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
        // First process should always fit
        assert(assignedAt != -1);
        processReadyPrint(0, nextProc, assignedAt);
        nextProc->memoryAssignAt = assignedAt;
    }

    // Add to the ready queue processes that arrive at the same time as the first
    nextProc = processRead(f);
    while (nextProc && nextProc->arrivalTime == curTime) {

        if (memory) {
            int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
            if (assignedAt != -1) {
                processReadyPrint(nextProc->arrivalTime, nextProc, assignedAt);
                nextProc->memoryAssignAt = assignedAt;
                llistAppend(queue, nextProc);
            } else {
                llistAppend(waiting, nextProc);
            }
        } else {
            llistAppend(queue, nextProc);
        }
        
        nextProc = processRead(f);

    }

    process_t* execProc = NULL;
    process_t* prevProc = NULL;

    while (queue->n > 0) {
        
        // Get the next process
        prevProc = execProc;
        execProc = llistPop(queue);
        if (execProc->serviceTime == execProc->remainTime) numProcesses++;

        // Run the process
        if (prevProc != execProc) processRunPrint(execProc, curTime);
        execProc->remainTime -= q;
        curTime += q;

        // Add all the jobs that have arrived
        while (nextProc && nextProc->arrivalTime <= curTime) {

            if (memory) {
                int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
                if (assignedAt != -1) {
                    processReadyPrint(nextProc->arrivalTime, nextProc, assignedAt);
                    nextProc->memoryAssignAt = assignedAt;
                    llistAppend(queue, nextProc);
                } else {
                    llistAppend(waiting, nextProc);
                }
            } else {
                llistAppend(queue, nextProc);
            }

            nextProc = processRead(f);

        }

        // Check to see if the process finished in this quantum
        if (execProc->remainTime <= 0) {
            
            processFinPrint(execProc, curTime,
                            queue->n + ((memory) ? waiting->n : 0));

            int curTurnaround = curTime - execProc->arrivalTime;
            totTurnaround += curTurnaround;
            double curOverhead = (double)curTurnaround/execProc->serviceTime;
            totOverhead += curOverhead;
            stats.maxOverhead = fmax(stats.maxOverhead, curOverhead);

            if (memory) memoryFree(memory, execProc->memoryAssignAt);
            processFree(execProc);

            // Now that there's more space, try to allocate memory to the waiting processes
            listNode_t* try = (memory) ? waiting->head : NULL;
            while (try) {

                int assignedAt = memoryAlloc(memory, ((process_t*)try->item)->memoryRequirement);

                if (assignedAt != -1) {

                    processReadyPrint(curTime, ((process_t*)try->item), assignedAt);
                    ((process_t*)try->item)->memoryAssignAt = assignedAt;

                    llistAppend(queue, ((process_t*)try->item));

                    listNode_t* temp = try;
                    if (try->prev) {
                        try->prev->next = try->next;
                    } else {
                        waiting->head = try->next;
                    }

                    waiting->n--;
                    try = temp->next;
                    free(temp);

                } else {
                    try = try->next;
                }

                
            }
            
        } else {

            llistAppend(queue, execProc);

        }

        // Skip "gaps" in time
        if (queue->n == 0 && nextProc) {

            int nextTime = nextProc->arrivalTime;
            curTime = (nextTime%q == 0) ? nextTime : (nextTime/q+1)*q;

            if (memory && nextProc) {
                int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
                if (assignedAt != -1) {
                    processReadyPrint(curTime, nextProc, assignedAt);
                    nextProc->memoryAssignAt = assignedAt;
                    llistAppend(queue, nextProc);
                } else {
                    llistAppend(waiting, nextProc);
                }
            } else {
                llistAppend(queue, nextProc);
            }

            nextProc = processRead(f);

        }

    }

    // Compute stats
    stats.turnaround = ceil((double)totTurnaround/numProcesses);
    stats.avgOverhead = totOverhead/numProcesses;
    stats.makespan = curTime;

    // Free real memory
    llistFree(queue);
    if (memory){
        llistFree(memory);
        llistFree(waiting);
    }

    return stats;

}

stats_t SJF(FILE* f, int q, char* memStrat) {

    linkedList_t* memory = NULL;
    linkedList_t* waiting = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        waiting = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }

    heap_t* heap = heapInit();
    stats_t stats = {};
    int numProcesses = 0, totTurnaround = 0;
    double totOverhead = 0;

    // Set the time to the start quantum of the first process
    process_t* nextProc = processRead(f);
    heapPush(heap, nextProc, processCompare);
    int curTime = (nextProc->arrivalTime%q == 0) ?
                   nextProc->arrivalTime : (nextProc->arrivalTime/q+1)*q;

    if (memory) {
        int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
        // First process should always fit
        assert(assignedAt != -1);
        processReadyPrint(0, nextProc, assignedAt);
        nextProc->memoryAssignAt = assignedAt;
    }

    // Add to the ready queue processes that arrive at the same time as the first
    nextProc = processRead(f);
    while (nextProc && nextProc->arrivalTime == curTime) {

        if (memory) {
            int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
            if (assignedAt != -1) {
                processReadyPrint(nextProc->arrivalTime, nextProc, assignedAt);
                nextProc->memoryAssignAt = assignedAt;
                heapPush(heap, nextProc, processCompare);
            } else {
                llistAppend(waiting, nextProc);
            }
        } else {
            heapPush(heap, nextProc, processCompare);
        }

        nextProc = processRead(f);

    }

    while (heap->n != 0) {
        
        // Get the shortest process
        process_t* execProc = heapPop(heap, processCompare);
        numProcesses++;

        // Run the process
        processRunPrint(execProc, curTime);

        // Calculate the finish time 
        int finTime = curTime + execProc->serviceTime;
        curTime = (finTime%q == 0) ? finTime : (finTime/q + 1) * q;

        // Add all the jobs that arrive strictly before the finish time
        while (nextProc && nextProc->arrivalTime < curTime-q) {

            if (memory) {
                int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
                if (assignedAt != -1) {
                    processReadyPrint(nextProc->arrivalTime, nextProc, assignedAt); ////time//////////////////////////////
                    nextProc->memoryAssignAt = assignedAt;
                    heapPush(heap, nextProc, processCompare);
                } else {
                    llistAppend(waiting, nextProc);
                }
            } else {
                heapPush(heap, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }        

        // Complete the running process and update stats
        processFinPrint(execProc, curTime,
                        heap->n + ((memory) ? waiting->n : 0));
        
        int curTurnaround = curTime - execProc->arrivalTime;
        totTurnaround += curTurnaround;
        double curOverhead = (double)curTurnaround/execProc->serviceTime;
        totOverhead += curOverhead;
        stats.maxOverhead = fmax(stats.maxOverhead, curOverhead);

        if (memory) memoryFree(memory, execProc->memoryAssignAt);
        processFree(execProc);

        // Now that there's more space, try to allocate memory to the waiting processes
        listNode_t* try = (memory) ? waiting->head : NULL;
        while (try) {
            
            int assignedAt = memoryAlloc(memory, ((process_t*)try->item)->memoryRequirement);

            if (assignedAt != -1) {

                processReadyPrint(curTime, ((process_t*)try->item), assignedAt);
                ((process_t*)try->item)->memoryAssignAt = assignedAt;

                heapPush(heap, ((process_t*)try->item), processCompare);

                listNode_t* temp = try;
                if (try->prev) {
                    try->prev->next = try->next;
                } else {
                    waiting->head = try->next;
                }
                
                waiting->n--;
                try = temp->next;
                
                free(temp);

            } else {
                try = try->next;
            }
            
        }

        // listNode_t* cur = memory->head;
        // while (cur) {
        //     printf("%c, %d, %d\n",((memBlock_t*)cur->item)->type,((memBlock_t*)cur->item)->start,((memBlock_t*)cur->item)->length);
        //     cur = cur->next;
        // }

        // Add all the jobs that arrive at the same time as the finish time
        while (nextProc && nextProc->arrivalTime <= curTime) {

            if (memory) {
                int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
                if (assignedAt != -1) {
                    processReadyPrint(curTime, nextProc, assignedAt);
                    nextProc->memoryAssignAt = assignedAt;
                    heapPush(heap, nextProc, processCompare);
                } else {
                    llistAppend(waiting, nextProc);
                }
            } else {
                heapPush(heap, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }

        // Skip "gaps" in time
        if (heap->n == 0 && nextProc) {

            int nextTime = nextProc->arrivalTime;
            curTime = (nextTime%q == 0) ? nextTime : (nextTime/q+1)*q;

            if (memory && nextProc) {
                int assignedAt = memoryAlloc(memory, nextProc->memoryRequirement);
                if (assignedAt != -1) {
                    processReadyPrint(curTime, nextProc, assignedAt);
                    nextProc->memoryAssignAt = assignedAt;
                    heapPush(heap, nextProc, processCompare);
                } else {
                    llistAppend(waiting, nextProc);
                }
            } else {
                heapPush(heap, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }

    }

    // Compute stats
    stats.turnaround = ceil((double)totTurnaround/numProcesses);
    stats.avgOverhead = totOverhead/numProcesses;
    stats.makespan = curTime;

    // Free real memory
    heapFree(heap);

    if (memory) {
        llistFree(memory);
        llistFree(waiting);
    }

    return stats;

}
