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

        free(proc->name);
        free(proc);
        return NULL;

    } else {

        // printf("%d, %s, %d, %d\n", proc->arrivalTime, proc->name, proc->serviceTime, proc->memoryRequirement);

        return proc;
        
    }

}

int processCompare(process_t* proc1, process_t* proc2) {

    if (proc1->serviceTime != proc2->serviceTime) {
        return proc1->serviceTime > proc2->serviceTime;
    } else if (proc1->arrivalTime != proc2->arrivalTime) {
        return proc1->arrivalTime > proc2->arrivalTime;
    } else {
        return strcmp(proc1->name, proc2->name) > 0;
    }

}

void processPrint(process_t* proc) {
    fprintf(stdout, "                %d, %s, %d, %d\n", proc->arrivalTime, proc->name, proc->serviceTime, proc->memoryRequirement);
}

void processRunPrint(process_t* proc, int time, int remainingTime) {
    fprintf(stdout,"%d,RUNNING,process_name=%s,remaining_time=%d\n",time,proc->name,remainingTime);
}

void processFinPrint(process_t* proc, int time, int procRemaining) {
    fprintf(stdout,"%d,FINISHED,process_name=%s,proc_remaining=%d\n",time,proc->name,procRemaining);
}