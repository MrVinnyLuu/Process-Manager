/*------------------------------------------------------------------------------
Vincent Luu, 1269979
--------------------------------------------------------------------------------
COMP30023 Project 1: Process Management
main.c : main program
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <getopt.h>

#include "record.h"
#include "heap.h"
#include "llist.h"
#include "memory.h"

#define IMPLEMENTS_REAL_PROCESS

/********************************** CONSTANTS *********************************/

#define MAX_MEMORY 2048
#define NUM_ENDIAN_BYTES 4
#define SHA_LEN 64

/********************************* STRUCTURES *********************************/

typedef struct stats {
    int numProcesses;
    int totTurnaround, avgTurnaround;
    double totOverhead, maxOverhead, avgOverhead;
    int makespan;
} stats_t;

/***************************** FUNCTION PROTOTYPES ****************************/

stats_t SJF(FILE* f, int q, char* memStrat);
stats_t RR(FILE* f, int q, char* memStrat);

void processCreate(int time, process_t* proc, int inputFD[2], int outputFD[2]);
void processCont(int time, process_t* proc);
void processSuspend(int time, process_t* proc);
char* processTerm(int time, process_t* proc);

void statsUpdate(int time, stats_t* stats, process_t* proc);
void statsFinalise(int time, stats_t* stats);

int roundq(int time, int quantum);
char* toBigEndian(int number);

/******************************************************************************/

int main(int argc, char* argv[]) {

    char* filepath = NULL;
    char* scheduler = NULL;
    char* memStrat = NULL;
    int quantum;
    
    // Read in settings
    int c;
    while ((c = getopt(argc, argv, ":f:s:m:q:")) != -1) {
        switch(c)  {
            case 'f':
                filepath = optarg;
                break;
            case 's':
                scheduler = optarg;
                break;
            case 'm':
                memStrat = optarg;
                break;
            case 'q':
                quantum = atoi(optarg);
                break;
        }
    }

    FILE* f = fopen(filepath, "r");
    assert(f);

    stats_t stats;

    // Choose scheduler
    if (strcmp(scheduler, "SJF") == 0) {
        stats = SJF(f, quantum, memStrat);
    } else if (strcmp(scheduler, "RR") == 0) {
        stats = RR(f, quantum, memStrat);
    }
    
    // Print stats
    printf("Turnaround time %d\n"
           "Time overhead %.2lf %.2lf\n"
           "Makespan %d\n",
           stats.avgTurnaround, roundf(stats.maxOverhead*100)/100,
           roundf(stats.avgOverhead*100)/100, stats.makespan);

    fclose(f);

    return 0;

}

/********************************** FUNCTIONS *********************************/

stats_t RR(FILE* f, int q, char* memStrat) {

    // Initialise
    stats_t stats = {0};
    linkedList_t* ready = llistInit();
    linkedList_t* input = llistInit();

    linkedList_t* memory = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }
    
    int inputFD[2], outputFD[2];

    // Ready in first process
    process_t* prevProc = NULL;
    process_t* execProc = NULL;
    process_t* nextProc = processRead(f);

    // First process always arrives at time 0 according to spec
    int curTime = 0;

    while (ready->n > 0 || input->n > 0 || nextProc || execProc) {

        // Check to see if the process finished in this quantum
        if (execProc && processRemainingTime(execProc) <= 0) {

            // Terminate the real process
            char* hash = processTerm(curTime, execProc);
            processFinPrint(curTime, execProc, ready->n + input->n, hash);
            free(hash);

            statsUpdate(curTime, &stats, execProc);

            if (memory) {
                memoryFree(memory, processMemoryAssignedAt(execProc));
            }

            processFree(execProc);
            execProc = NULL;
            
        }

        // Add all the jobs that have arrived to input queue
        while (nextProc && processArrivalTime(nextProc) <= curTime) {
            llistAppend(input, nextProc);
            nextProc = processRead(f);
        }

        // Try allocate memory to all processes in input queue
        int n = input->n;
        for (int i = 0; i < n; i++) {
            process_t* proc = memoryAssign(curTime, memory, input);
            // If successful memory allocation, move to ready queue
            if (proc) llistAppend(ready, proc);
        }

        // If running process is not finished:
        if (execProc) {
            // Suspend the real process if there are other processes to be run
            if (ready->n > 0) processSuspend(curTime, execProc);
            // Re-queue
            llistAppend(ready, execProc);
        }
    
        // Get and run the next process (if there are any)
        if (ready->n > 0) {

            prevProc = execProc;
            execProc = llistPop(ready);
            if (prevProc != execProc) processRunPrint(curTime, execProc);

            // Check if this is an unstarted process
            if (processServiceTime(execProc) ==
                processRemainingTime(execProc)) {

                // Create and execute a real process
                processCreate(curTime, execProc, inputFD, outputFD);
                stats.numProcesses++;

            } else {

                // Continue the real process
                processCont(curTime, execProc);
                
            }

            processIncrement(execProc, q);

        }
        
        // Increment time
        curTime += q;

        // Skip "gaps" in time
        if (nextProc && ready->n == 0 && !execProc) {
            curTime = roundq(processArrivalTime(nextProc), q);
            llistAppend(input, nextProc);
            nextProc = processRead(f);
        }

    }

    // Compute stats
    statsFinalise(curTime-q, &stats);

    // Free real memory
    llistFree(ready);
    llistFree(input);
    if (memory) llistFree(memory);

    return stats;

}

stats_t SJF(FILE* f, int q, char* memStrat) {
    
    // Initialise
    stats_t stats = {0};
    heap_t* ready = heapInit();
    linkedList_t* input = llistInit();

    linkedList_t* memory = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }

    int inputFD[2], outputFD[2];

    // Ready in first process
    process_t* execProc = NULL;
    process_t* nextProc = processRead(f);

    // First process always arrives at time 0 according to spec
    int curTime = 0;

    while (ready->n > 0 || input->n > 0 || nextProc || execProc) {

        if (execProc) {

            processIncrement(execProc, q);
            
            // Check if running process has completed
            if (processRemainingTime(execProc) <= 0) {
                
                // Terminate the real process
                char* hash = processTerm(curTime, execProc);
                
                processFinPrint(curTime, execProc, ready->n + input->n, hash);
                free(hash);
                
                statsUpdate(curTime, &stats, execProc);

                if (memory) {
                    memoryFree(memory, processMemoryAssignedAt(execProc));
                }

                processFree(execProc);
                execProc = NULL;

            } else {
                
                // Continue the real process
                processCont(curTime, execProc);

            }

        }
        
        // Add all the jobs that arrive to input queue
        while (nextProc && processArrivalTime(nextProc) <= curTime) {
            llistAppend(input, nextProc);
            nextProc = processRead(f);
        }

        // Try allocate memory to all processes in input queue
        int n = input->n;
        for (int i = 0; i < n; i++) {
            process_t* proc = memoryAssign(curTime, memory, input);
            // If successful memory allocation, move to ready queue
            if (proc) heapPush(ready, proc, processCompare);
        }

        // If no proc running, get and run shortest process (if there are any)
        if (!execProc && ready->n > 0) {

            execProc = heapPop(ready, processCompare);
            processRunPrint(curTime, execProc);

            // Create and execute a real process
            processCreate(curTime, execProc, inputFD, outputFD);

            

            stats.numProcesses++;
            
        }

        // Increment time
        curTime += q;

        // Skip "gaps" in time
        if (nextProc && ready->n == 0 && !execProc) {
            curTime = roundq(processArrivalTime(nextProc), q);
            llistAppend(input, nextProc);
            nextProc = processRead(f);
        }

    }

    statsFinalise(curTime-q, &stats);
    
    // Free real memory
    heapFree(ready);
    llistFree(input);
    if (memory) llistFree(memory);

    return stats;

}

/*                         CONTROLLING REAL PROCESSES                         */

void processCreate(int time, process_t* proc, int inputFD[2], int outputFD[2]) {
    
    // Set up and check pipes
    assert(pipe(inputFD) == 0 && pipe(outputFD) == 0);

    // Fork and check
    pid_t childPID;
    assert((childPID = fork()) != -1);

    if (childPID == 0) {

        close(inputFD[1]);
        dup2(inputFD[0], STDIN_FILENO);
        close(inputFD[0]);

        close(outputFD[0]);
        dup2(outputFD[1], STDOUT_FILENO);
        close(outputFD[1]);

        // Execute child process
        char *args[] = {"./process", processName(proc), NULL};
        execvp(args[0], args);

        // execvp only returns if unsucessful or if there was an error
        exit(EXIT_FAILURE);

    } else {

        processSetReal(proc, childPID, inputFD[1], outputFD[0]);

        close(inputFD[0]);
        close(outputFD[1]);

        // Send start time to child process
        char *timeBytes = toBigEndian(time);
        write(inputFD[1], timeBytes, NUM_ENDIAN_BYTES);

        // Read 1 byte
        char readByte[1];
        read(outputFD[0], readByte, 1);

        // Verify read byte is the same as last byte send
        assert(readByte[0] == timeBytes[NUM_ENDIAN_BYTES-1]);
        free(timeBytes);

    }

}

void processCont(int time, process_t* proc) {
    
    // Write continued time to child process
    char* timeBytes = toBigEndian(time);
    write(processReadInFD(proc), timeBytes, NUM_ENDIAN_BYTES);
    
    kill(processRealPID(proc), SIGCONT);
    
    // Read 1 byte
    char readByte[1];
    read(processWriteOutFD(proc), readByte, 1);

    // Verify read byte is the same as last byte send
    assert(readByte[0] == timeBytes[NUM_ENDIAN_BYTES-1]);
    free(timeBytes);

}

void processSuspend(int time, process_t* proc) {
    
    // Write suspended time to child process
    char* timeBytes = toBigEndian(time);
    write(processReadInFD(proc), timeBytes, NUM_ENDIAN_BYTES);
    free(timeBytes);
    
    kill(processRealPID(proc), SIGTSTP);

    // Wait for process to enter a stopped state
    int wstatus;
    pid_t w;
    do {
        w = waitpid(processRealPID(proc), &wstatus, WUNTRACED);
        assert(w != -1);
    } while (!WIFSTOPPED(wstatus));

}

char* processTerm(int time, process_t* proc) {
    
    // Write termination time to child process
    char* timeBytes = toBigEndian(time);
    write(processReadInFD(proc), timeBytes, NUM_ENDIAN_BYTES);
    free(timeBytes);

    kill(processRealPID(proc), SIGTERM);
    
    // Read and print the hash
    char* hash = malloc(SHA_LEN+1);
    assert(hash);
    read(processWriteOutFD(proc), hash, SHA_LEN);
    hash[SHA_LEN] = '\0'; // End the string with null byte
    
    return hash;

}

void statsUpdate(int time, stats_t* stats, process_t* proc) {
    int curTurnaround = time - processArrivalTime(proc);
    stats->totTurnaround += curTurnaround;
    double curOverhead = (double)curTurnaround/processServiceTime(proc);
    stats->totOverhead += curOverhead;
    stats->maxOverhead = fmax(stats->maxOverhead, curOverhead);
}

void statsFinalise(int time, stats_t* stats) {
    if (stats->numProcesses == 0) return;
    stats->avgTurnaround = 
        ceil((double)stats->totTurnaround/stats->numProcesses);
    stats->avgOverhead = stats->totOverhead/stats->numProcesses;
    stats->makespan = time;
}

/*                              HELPER FUNCTIONS                              */

/* "Round up" to the nearest multiple of quantum */
int roundq(int time, int quantum) {
    return (time%quantum == 0) ? time : (time/quantum+1)*quantum;
}

/* Convert 32-bit integer into Big Endian Btye Ordering */
char* toBigEndian(int number) {

    int* p = &number;

    char* bytePtr = (char*)p;

    char* bytes = malloc(NUM_ENDIAN_BYTES);
    assert(bytes);

    for (int i = 0; i < NUM_ENDIAN_BYTES; i++) {
        bytes[i] = bytePtr[NUM_ENDIAN_BYTES-1-i];
    }

    return bytes;

}

/******************************************************************************/