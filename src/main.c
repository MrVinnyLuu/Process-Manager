/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
main.c : main program
------------------------------------------------------------------------------*/

/*
The order in Section 2 of the specification must always to be followed, regardless of the scheduling algorithm. 

To summarise, that is:

1. Process termination (if applicable). 
2. Adding incoming processes to the input queue. 
3. Memory allocation. If allocated successfully, moving the process to the ready queue. 
4. Scheduling decision. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <getopt.h>
#include <inttypes.h>

#include "record.h"
#include "heap.h"
#include "llist.h"
#include "memory.h"

#define IMPLEMENTS_REAL_PROCESS

#define MAX_MEMORY 2048

typedef struct stats {
    int numProcesses;
    int totTurnaround, avgTurnaround;
    double totOverhead, maxOverhead, avgOverhead;
    int makespan;
} stats_t;

stats_t SJF(FILE* f, int q, char* memStrat);
stats_t RR(FILE* f, int q, char* memStrat);
int roundq(int time, int quantum);
void statsUpdate(int time, stats_t* stats, process_t* proc);
void statsFinalise(int time, stats_t* stats);

int main(int argc, char* argv[]) {

    // char* filepath = NULL;
    // char* scheduler = NULL;
    // char* memStrat = NULL;
    // int quantum;
    
    // // Read in settings
    // int c;
    // while ((c = getopt(argc, argv, ":f:s:m:q:")) != -1) {
    //     switch(c)  {
    //         case 'f':
    //             filepath = optarg;
    //             break;
    //         case 's':
    //             scheduler = optarg;
    //             break;
    //         case 'm':
    //             memStrat = optarg;
    //             break;
    //         case 'q':
    //             quantum = atoi(optarg);
    //             break;
    //     }
    // }

    // printf("file: %s\nscheduler: %s\nmemstrat: %s\nquantum: %d\n", filepath, scheduler, memStrat, quantum);

    // FILE* f = fopen(filepath, "r");
    // assert(f);

    // stats_t stats;

    // // Choose scheduler
    // if (strcmp(scheduler, "SJF") == 0) {
    //     stats = SJF(f, quantum, memStrat);
    // } else if (strcmp(scheduler, "RR") == 0) {
    //     stats = RR(f, quantum, memStrat);
    // }
    
    // // Print stats
    // printf("Turnaround time %d\n"
    //        "Time overhead %.2lf %.2lf\n"
    //        "Makespan %d\n",
    //        stats.avgTurnaround, roundf(stats.maxOverhead*100)/100,
    //        roundf(stats.avgOverhead*100)/100, stats.makespan);

    // fclose(f);

    // return 0;

    int inputPipe[2];
    assert(pipe(inputPipe) == 0);

    int outputPipe[2];
    assert(pipe(outputPipe) == 0);

    pid_t pid = fork();
    assert(pid != -1);

    // This is the child process
    if (pid == 0) {
        
        // Close write end of input pipe
        close(inputPipe[1]);

        // Duplicate read end of pipe to stdin
        dup2(inputPipe[0], STDIN_FILENO);

        // Close read end of input pipe
        close(inputPipe[0]);


        // Close read end of output pipe
        close(outputPipe[0]);

        // Duplicate write end of pipe to stdout
        dup2(outputPipe[1], STDOUT_FILENO);

        // Close write end of output pipe
        close(outputPipe[1]);

        // Execute child process
        char *args[] = {"./process", "P", "-v", NULL};
        execvp(args[0], args);

        // execvp only returns if there was an error
        perror("execvp");
        exit(EXIT_FAILURE);

    // This is the parent process
    } else {
        
        // Close read end of input pipe
        close(inputPipe[0]);

        // Close write end of output pipe
        close(outputPipe[1]);

        // Send data to child process
        char *data = "\x00\x00\x02\x80";
        write(inputPipe[1], data, 4);

        // Read data from child process
        char outputBuffer[1024];
        read(outputPipe[0], outputBuffer, 1);
        printf("Received from child process: %c\n", outputBuffer[0]);

        // Write data to child process
        data = "\x00\x00\x02\xBC";
        write(inputPipe[1], data, 4);
        
        // End ./process
        kill(pid, SIGTERM);

        // Read the hash
        read(outputPipe[0], outputBuffer, 64);
        printf("Received from child process: %.64s\n", outputBuffer);        

        int status= 1;
        wait(&status);
        printf("Child process exited with status %d\n", status);
        
    }

    return 0;

}

/* "Round up" to the nearest multiple of quantum */
int roundq(int time, int quantum) {
    return (time%quantum == 0) ? time : (time/quantum+1)*quantum;
}




// printf '\x00\x00\x02\x80' | ./process P -v

stats_t RR(FILE* f, int q, char* memStrat) {

    // Initialise structs
    linkedList_t* queue = llistInit();

    linkedList_t* memory = NULL;
    linkedList_t* waiting = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        waiting = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }
    
    stats_t stats = {0};

    // Set the time to the start quantum of the first process
    process_t* nextProc = processRead(f);
    if (!nextProc) return stats;
    int curTime = roundq(nextProc->arrivalTime, q);

    if (memory) {
        llistAppend(queue,
            memoryAssign(curTime, memory, waiting, nextProc));
    } else {
        llistAppend(queue, nextProc);
    }

    // Add to ready queue processes that arrive at the same quantum as the first
    nextProc = processRead(f);
    while (nextProc && nextProc->arrivalTime <= curTime) {

        if (memory) {
            llistAppend(queue, 
                memoryAssign(curTime, memory, waiting, nextProc));
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
        if (execProc->serviceTime == execProc->remainTime) stats.numProcesses++;

        // Run the process
        if (prevProc != execProc) processRunPrint(curTime, execProc);
        execProc->remainTime -= q;
        curTime += q;

        // Add all the jobs that have arrived
        while (nextProc && nextProc->arrivalTime <= curTime) {

            if (memory) {
                llistAppend(queue,
                    memoryAssign(roundq(nextProc->arrivalTime,q), memory, waiting, nextProc));
            } else {
                llistAppend(queue, nextProc);
            }

            nextProc = processRead(f);

        }

        // Check to see if the process finished in this quantum
        if (execProc->remainTime <= 0) {
            
            processFinPrint(curTime, execProc,
                            queue->n + ((memory) ? waiting->n : 0));

            statsUpdate(curTime, &stats, execProc);

            if (memory) memoryFree(memory, execProc->memoryAssignAt);
            processFree(execProc);

            // Now that there's more space, try to allocate memory to the waiting processes
            listNode_t* try = (memory) ? waiting->head : NULL;
            while (try) {
                process_t* proc = memoryRetry(curTime, memory, waiting, &try);
                if (proc) llistAppend(queue, proc);
            }
            
        } else {

            llistAppend(queue, execProc);

        }

        // Skip "gaps" in time
        if (queue->n == 0 && nextProc) {

            curTime = roundq(nextProc->arrivalTime, q);

            if (memory && nextProc) {
                llistAppend(queue,
                    memoryAssign(curTime, memory, waiting, nextProc));
            } else {
                llistAppend(queue, nextProc);
            }

            nextProc = processRead(f);

        }

    }

    // Compute stats
    statsFinalise(curTime, &stats);

    // Free real memory
    llistFree(queue);
    if (memory){
        llistFree(memory);
        llistFree(waiting);
    }

    return stats;

}









stats_t SJF(FILE* f, int q, char* memStrat) {

    // Initialise structs
    heap_t* heap = heapInit();

    linkedList_t* memory = NULL;
    linkedList_t* waiting = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        waiting = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }

    stats_t stats = {0};

    // Set the time to the start quantum of the first process
    process_t* nextProc = processRead(f);
    if (!nextProc) return stats;
    int curTime = roundq(nextProc->arrivalTime, q);

    if (memory) {
        heapPush(heap,
                 memoryAssign(curTime, memory, waiting, nextProc),
                 processCompare);
    } else {
        heapPush(heap, nextProc, processCompare);
    }

    // Add to ready queue processes that arrive at the same time as the first
    nextProc = processRead(f);
    while (nextProc && nextProc->arrivalTime == curTime) {

        if (memory) {
            heapPush(heap,
                memoryAssign(curTime, memory, waiting, nextProc),
                processCompare);
        } else {
            heapPush(heap, nextProc, processCompare);
        }

        nextProc = processRead(f);

    }

    while (heap->n > 0) {
        
        // Get the shortest process
        process_t* execProc = heapPop(heap, processCompare);
        stats.numProcesses++;

        // Run the process
        processRunPrint(curTime, execProc);

        // Calculate the finish time 
        curTime = roundq(curTime + execProc->serviceTime, q);

        // Add all the jobs that arrive strictly before the finish time
        while (nextProc && nextProc->arrivalTime < curTime-q) {

            if (memory) {
                heapPush(heap,
                    memoryAssign(roundq(nextProc->arrivalTime,q), memory, waiting, nextProc),
                    processCompare);
            } else {
                heapPush(heap, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }        

        // Complete the running process
        processFinPrint(curTime, execProc,
                        heap->n + ((memory) ? waiting->n : 0));
        
        statsUpdate(curTime, &stats, execProc);

        if (memory) memoryFree(memory, execProc->memoryAssignAt);
        processFree(execProc);

        // Now there's more space, try allocate memory to the waiting processes
        listNode_t* try = (memory) ? waiting->head : NULL;
        while (try) {
            process_t* proc = memoryRetry(curTime, memory, waiting, &try);
            if (proc) heapPush(heap, proc, processCompare);
        }

        // listNode_t* cur = memory->head;
        // while (cur) {
        //     printf("%c, %d, %d\n",((memBlock_t*)cur->item)->type,((memBlock_t*)cur->item)->start,((memBlock_t*)cur->item)->length);
        //     cur = cur->next;
        // }

        // Add all the jobs that arrive at the same time as the finish time
        while (nextProc && nextProc->arrivalTime <= curTime) {

            if (memory) {
                heapPush(heap,
                    memoryAssign(curTime, memory, waiting, nextProc),
                    processCompare);
            } else {
                heapPush(heap, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }

        // Skip "gaps" in time
        if (heap->n == 0 && nextProc) {

            curTime = roundq(nextProc->arrivalTime, q);

            if (memory && nextProc) {
                heapPush(heap,
                    memoryAssign(curTime, memory, waiting, nextProc),
                    processCompare);
            } else {
                heapPush(heap, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }

    }

    statsFinalise(curTime, &stats);
    
    // Free real memory
    heapFree(heap);

    if (memory) {
        llistFree(memory);
        llistFree(waiting);
    }

    return stats;

}

void statsUpdate(int time, stats_t* stats, process_t* proc) {

    int curTurnaround = time - proc->arrivalTime;
    stats->totTurnaround += curTurnaround;

    double curOverhead = (double)curTurnaround/proc->serviceTime;
    stats->totOverhead += curOverhead;
    stats->maxOverhead = fmax(stats->maxOverhead, curOverhead);
    
}

void statsFinalise(int time, stats_t* stats) {
    stats->avgTurnaround =
        ceil((double)stats->totTurnaround/stats->numProcesses);
    stats->avgOverhead = stats->totOverhead/stats->numProcesses;
    stats->makespan = time;
}


