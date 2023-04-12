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
#define NUM_ENDIAN_BYTES 4
#define SHA_LEN 64

typedef struct stats {
    int numProcesses;
    int totTurnaround, avgTurnaround;
    double totOverhead, maxOverhead, avgOverhead;
    int makespan;
} stats_t;

stats_t SJF(FILE* f, int q, char* memStrat);
stats_t RR(FILE* f, int q, char* memStrat);

int roundq(int time, int quantum);
char* toBigEndian(int number);

void processCreate(int time, process_t* proc, int inputFD[2], int outputFD[2]);
void processCont(int time, process_t* proc, int inputFD[2], int outputFD[2]);
void processSuspend(int time, process_t* proc, int inputFD[2], int outputFD[2]);
void processTerm(int time, process_t* proc, int inputFD[2], int outputFD[2]);

void statsUpdate(int time, stats_t* stats, process_t* proc);
void statsFinalise(int time, stats_t* stats);

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

    // printf("file: %s\nscheduler: %s\nmemstrat: %s\nquantum: %d\n", filepath, scheduler, memStrat, quantum);

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

    // int inputFD[2];
    // assert(pipe(inputFD) == 0);

    // int outputFD[2];
    // assert(pipe(outputFD) == 0);

    // pid_t pid = fork();
    // assert(pid != -1);

    // // This is the child process
    // if (pid == 0) {
        
    //     // Close write end of input pipe
    //     close(inputFD[1]);

    //     // Duplicate read end of pipe to stdin
    //     dup2(inputFD[0], STDIN_FILENO);

    //     // Close read end of input pipe
    //     close(inputFD[0]);


    //     // Close read end of output pipe
    //     close(outputFD[0]);

    //     // Duplicate write end of pipe to stdout
    //     dup2(outputFD[1], STDOUT_FILENO);

    //     // Close write end of output pipe
    //     close(outputFD[1]);

    //     // Execute child process
    //     char *args[] = {"./process", "P4", "-v", NULL};
    //     execvp(args[0], args);

    //     // execvp only returns if there was an error
    //     perror("execvp");
    //     exit(EXIT_FAILURE);

    // // This is the parent process
    // } else {
        
    //     // Close read end of input pipe
    //     close(inputFD[0]);

    //     // Close write end of output pipe
    //     close(outputFD[1]);

    //     // Send data to child process
    //     char *data = toBigEndian(50);
    //     write(inputFD[1], data, 4);

    //     // Read data from child process
    //     char outputBuffer[1024];
    //     read(outputFD[0], outputBuffer, 1);
    //     printf("Received from child process: %c\n", outputBuffer[0]);

    //     for (int time = 51; time < 60; time++) {
    //         data = toBigEndian(time);
    //         write(inputFD[1], data, 4);
    //         kill(pid,SIGCONT);
    //         read(outputFD[0], outputBuffer, 64);
    //         printf("Received from child process: %.64s\n", outputBuffer);
    //     }

    //     // Write data to child process
    //     data = toBigEndian(60);
    //     write(inputFD[1], data, 4);
        
    //     // End ./process
    //     kill(pid, SIGTERM);

    //     // Read the hash
    //     read(outputFD[0], outputBuffer, 64);
    //     printf("Received from child process: %.64s\n", outputBuffer);        

    //     int status= 1;
    //     wait(&status);
    //     printf("Child process exited with status %d\n", status);
        
    // }

    // return 0;

}

/* "Round up" to the nearest multiple of quantum */
int roundq(int time, int quantum) {
    return (time%quantum == 0) ? time : (time/quantum+1)*quantum;
}

/* Convert 32-bit integer into big endian */
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

void processCreate(int time, process_t* proc, int inputFD[2], int outputFD[2]) {
    
    // Set up and check pipes
    assert(pipe(inputFD) == 0 && pipe(outputFD) == 0);

    // Fork and check
    pid_t childPID;
    assert((childPID = fork()) != -1);

    if (childPID == 0) {
        
        // Close write end of input pipe
        close(inputFD[1]);

        // Duplicate read end of pipe to stdin
        dup2(inputFD[0], STDIN_FILENO);

        // Close read end of input pipe
        close(inputFD[0]);

        // Close read end of output pipe
        close(outputFD[0]);

        // Duplicate write end of pipe to stdout
        dup2(outputFD[1], STDOUT_FILENO);

        // Close write end of output pipe
        close(outputFD[1]);

        // Execute child process
        char *args[] = {"./process", "-v", proc->name, NULL};
        execvp(args[0], args);

        // execvp only returns if unsucessful or in there was an error
        exit(EXIT_FAILURE);

    } else {

        proc->realPID = childPID;

        // Close read end of input pipe
        close(inputFD[0]);

        // Close write end of output pipe
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

void processCont(int time, process_t* proc, int inputFD[2], int outputFD[2]) {
    
    // Write continued time to child process
    char* timeBytes = toBigEndian(time);
    write(inputFD[1], timeBytes, NUM_ENDIAN_BYTES);
    
    kill(proc->realPID, SIGCONT);
    
    // Read 1 byte
    char readByte[1];
    read(outputFD[0], readByte, 1);

    // Verify read byte is the same as last byte send
    assert(readByte[0] == timeBytes[NUM_ENDIAN_BYTES-1]);
    free(timeBytes);

}

void processSuspend(int time, process_t* proc, int inputFD[2], int outputFD[2]) {
    
    // Write suspended time to child process
    char* timeBytes = toBigEndian(time);
    write(inputFD[1], timeBytes, NUM_ENDIAN_BYTES);
    
    kill(proc->realPID, SIGTSTP);

    // Wait for process to enter a stopped state
    int wstatus;
    pid_t w;
    do {
        w = waitpid(proc->realPID, &wstatus, WUNTRACED);
        assert(w != -1);
    } while (!WIFSTOPPED(wstatus));

}

void processTerm(int time, process_t* proc, int inputFD[2], int outputFD[2]) {
    
    // Write termination time to child process
    char* timeBytes = toBigEndian(time);
    write(inputFD[1], timeBytes, NUM_ENDIAN_BYTES);
    free(timeBytes);

    kill(proc->realPID, SIGTERM);
    
    // Read the hash
    char hash[SHA_LEN];
    read(outputFD[0], hash, SHA_LEN);
    processSHAPrint(time, proc, hash);

}






// printf '\x00\x00\x02\x80' | ./process P -v

stats_t RR(FILE* f, int q, char* memStrat) {

    // Initialise
    stats_t stats = {0};
    linkedList_t* ready = llistInit();

    linkedList_t* memory = NULL;
    linkedList_t* waiting = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        waiting = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }
    
    int inputFD[2], outputFD[2];

    // Set the time to the start quantum of the first process
    process_t* execProc = NULL;
    process_t* prevProc = NULL;
    process_t* nextProc = processRead(f);
    if (!nextProc) return stats;
    int curTime = roundq(nextProc->arrivalTime, q);

    if (memory) {
        llistAppend(ready, memoryAssign(curTime, memory, waiting, nextProc));
    } else {
        llistAppend(ready, nextProc);
    }

    nextProc = processRead(f);

    while (ready->n > 0 || nextProc || execProc) {

        // Check to see if the process finished in this quantum
        if (execProc && execProc->remainingTime <= 0) {
            
            processFinPrint(curTime, execProc,
                            ready->n + ((memory) ? waiting->n : 0));

            statsUpdate(curTime, &stats, execProc);

            // Terminate the real process
            processTerm(curTime, execProc, inputFD, outputFD);

            if (memory) memoryFree(memory, execProc->memoryAssignAt);
            processFree(execProc);
            execProc = NULL;

            // Now there's more space, try allocate memory to waiting processes
            listNode_t* try = (memory) ? waiting->head : NULL;
            while (try) {
                process_t* proc = memoryRetry(curTime, memory, waiting, &try);
                if (proc) llistAppend(ready, proc);
            }
            
        }

        // Add all the jobs that have arrived
        while (nextProc && nextProc->arrivalTime <= curTime) {

            if (memory) {
                llistAppend(ready, 
                    memoryAssign(roundq(nextProc->arrivalTime,q), memory, waiting, nextProc));
            } else {
                llistAppend(ready, nextProc);
            }

            nextProc = processRead(f);

        }

        if (execProc && ready->n > 0) {

            // Suspend the real process
            processSuspend(curTime, execProc, inputFD, outputFD);

        }

        // Re-queue the running process if not finished
        if (execProc) llistAppend(ready, execProc);

        

        // Get and run the next process (if there are any)
        prevProc = execProc;
        if (ready->n > 0)  {

            execProc = llistPop(ready);

            // Check if this is an unstarted process
            if (execProc->serviceTime == execProc->remainingTime) {
                // Create and execute a real process
                processCreate(curTime, execProc, inputFD, outputFD);
                stats.numProcesses++;
            } else {
                // Continue the real process
                processCont(curTime, execProc, inputFD, outputFD);
            }

            if (prevProc != execProc) processRunPrint(curTime, execProc);
            execProc->remainingTime -= q;

        }
        
        // Increment time
        curTime += q;

        // Skip "gaps" in time
        if (ready->n == 0 && nextProc && !execProc) {

            curTime = roundq(nextProc->arrivalTime, q);

            if (memory && nextProc) {
                llistAppend(ready,
                    memoryAssign(curTime, memory, waiting, nextProc));
            } else {
                llistAppend(ready, nextProc);
            }

            nextProc = processRead(f);

        }

    }

    // Compute stats
    statsFinalise(curTime-q, &stats);

    // Free real memory
    llistFree(ready);
    if (memory){
        llistFree(memory);
        llistFree(waiting);
    }

    return stats;

}





stats_t SJF(FILE* f, int q, char* memStrat) {

    // Initialise
    stats_t stats = {0};
    heap_t* ready = heapInit();

    linkedList_t* memory = NULL;
    linkedList_t* waiting = NULL;
    if (strcmp(memStrat, "best-fit") == 0) {
        memory = llistInit();
        waiting = llistInit();
        memoryInit(memory, MAX_MEMORY);
    }

    int inputFD[2], outputFD[2];

    // Set the time to the start quantum of the first process
    process_t* execProc = NULL;
    process_t* nextProc = processRead(f);
    if (!nextProc) return stats;
    int curTime = roundq(nextProc->arrivalTime, q);

    if (memory) {
        heapPush(ready,
                 memoryAssign(curTime, memory, waiting, nextProc),
                 processCompare);
    } else {
        heapPush(ready, nextProc, processCompare);
    }

    nextProc = processRead(f);

    while (ready->n > 0 || nextProc || execProc) {

        if (execProc) execProc->remainingTime -= q;

        // Check if running process has completed
        if (execProc && execProc->remainingTime <= 0) {
            
            processFinPrint(curTime, execProc,
                            ready->n + ((memory) ? waiting->n : 0));
            
            statsUpdate(curTime, &stats, execProc);

            // Terminate the real process
            processTerm(curTime, execProc, inputFD, outputFD);

            if (memory) memoryFree(memory, execProc->memoryAssignAt);
            processFree(execProc);
            execProc = NULL;

            // Now there's more space, try allocate memory to waiting processes
            listNode_t* try = (memory) ? waiting->head : NULL;
            while (try) {
                process_t* proc = memoryRetry(curTime, memory, waiting, &try);
                if (proc) heapPush(ready, proc, processCompare);
            }

        } else if (execProc) {
            
            // Continue the real process
            processCont(curTime, execProc, inputFD, outputFD);

        }

        // Add all the jobs that arrive
        while (nextProc && nextProc->arrivalTime <= curTime) {

            if (memory) {
                heapPush(ready,
                    memoryAssign(roundq(nextProc->arrivalTime,q), memory, waiting, nextProc),
                    processCompare);
            } else {
                heapPush(ready, nextProc, processCompare);
            }

            nextProc = processRead(f);

        } 

        // If no proc running, get and run shortest process (if there are any)
        if (!execProc && ready->n > 0) {

            execProc = heapPop(ready, processCompare);
            stats.numProcesses++;
            processRunPrint(curTime, execProc);

            // Create and execute a real process
            processCreate(curTime, execProc, inputFD, outputFD);

        }

        // Increment time
        curTime += q;

        // Skip "gaps" in time
        if (ready->n == 0 && nextProc && !execProc) {

            curTime = roundq(nextProc->arrivalTime, q);

            if (memory && nextProc) {
                heapPush(ready,
                    memoryAssign(curTime, memory, waiting, nextProc),
                    processCompare);
            } else {
                heapPush(ready, nextProc, processCompare);
            }

            nextProc = processRead(f);

        }

    }

    statsFinalise(curTime-q, &stats);
    
    // Free real memory
    heapFree(ready);
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
