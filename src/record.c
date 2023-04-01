#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "record.h"

#define MAX_NAME_LEN 8

// process_t* processRead(FILE* f) {

//     size_t bufsize = MAX_LINE_LEN+1;
// 	char *record = malloc(bufsize * sizeof(*record));
// 	assert(record);

//     int len = getdelim(&record, &bufsize, '\n', f);
// 	if (len == -1) {
// 		free(record);
// 		return NULL;
// 	}
	
// 	process_t* proc = malloc(sizeof(*proc));
// 	assert(proc);

//     int field = 0;
//     char* ptr = NULL;

//     ptr = strtok(record, " ");
//     proc->arrivalTime = atoi(ptr);

//     while (NULL != (ptr = strtok(NULL, " "))) {

//         field++;

//         switch (field) {
//             case 1:
//                 proc->name = ptr;
//             case 2:
//                 proc->serviceTime = atoi(ptr);
//             case 3:
//                 proc->memoryRequirement = atoi(ptr);
//         }

//     }

//     // printf("%d, %s, %d, %d\n", proc->arrivalTime, proc->name, proc->serviceTime, proc->memoryRequirement);

//     return proc;

// }

process_t* processRead(FILE* f) {

    process_t* proc = malloc(sizeof(*proc));
    proc->name = malloc(MAX_NAME_LEN);
	assert(proc);

    if (fscanf(f, "%d %s %d %d",
               &proc->arrivalTime,
               proc->name,
               &proc->serviceTime,
               &proc->memoryRequirement) != 4) {

        processFree(proc);
        return NULL;

    } else {

        proc->remainTime = proc->serviceTime;
        return proc;

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

void processPrint(process_t* proc) {
    fprintf(stdout, "                %d, %s, %d, %d\n", proc->arrivalTime, proc->name, proc->serviceTime, proc->memoryRequirement);
}

void processRunPrint(process_t* proc, int time) {
    fprintf(stdout,"%d,RUNNING,process_name=%s,remaining_time=%d\n",
            time, proc->name, proc->remainTime);
}

void processFinPrint(process_t* proc, int time, int procRemaining) {
    fprintf(stdout,"%d,FINISHED,process_name=%s,proc_remaining=%d\n",
            time, proc->name, procRemaining);
}

void processMemoryPrint(int time, process_t* proc, int assignAt) {
    fprintf(stdout,"%d,READY,process_name=%s,assigned_at=%d\n",
            time, proc->name, assignAt);
}

void processFree(process_t* proc) {
    free(proc->name);
    free(proc);
}