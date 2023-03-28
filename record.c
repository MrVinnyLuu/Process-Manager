#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "record.h"

#define MAX_PROC_NAME_LEN 8
#define MAX_LINE_LEN (10+MAX_PROC_NAME_LEN+10+4+4)

process_t* processRead(FILE* f) {

    size_t bufsize = MAX_LINE_LEN+1;
	char *record = malloc(bufsize * sizeof(*record));
	assert(record);

    int len = getdelim(&record, &bufsize, '\n', f);
	if (len == -1) {
		free(record);
		return NULL;
	}
	
	process_t* proc = malloc(sizeof(*proc));
	assert(proc);

    proc->arrivalTime = 1;
    proc->name = "Hello";
    proc->serviceTime = 3;
    proc->memoryRequirement = 4;

    int field = 0;
    char* ptr = NULL;

    ptr = strtok(record, " ");
    proc->arrivalTime = atoi(ptr);

    while (NULL != (ptr = strtok(NULL, " "))) {

        field++;

        switch (field) {
            case 0:
                proc->arrivalTime = atoi(ptr);
            case 1:
                proc->name = ptr;
            case 2:
                proc->serviceTime = atoi(ptr);
            case 3:
                proc->memoryRequirement = atoi(ptr);
        }

    }

    // printf("%d, %s, %d, %d\n", proc->arrivalTime, proc->name, proc->serviceTime, proc->memoryRequirement);

    return proc;

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