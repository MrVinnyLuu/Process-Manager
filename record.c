#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "record.h"

#define MAX_PROC_NAME_LEN 8
#define MAX_LINE_LEN (10+MAX_PROC_NAME_LEN+10+4+4)

struct process {
    int arrivalTime;
    char* name;
    int serviceTime;
    int memoryRequirement;
};

void process_read(FILE* f) {

    process_t record = {0, NULL, 0, 0};

    int i = 0;

    char line[MAX_LINE_LEN] = {0};
    char *ptr = NULL;

    record.name = malloc(MAX_PROC_NAME_LEN);
    assert(record.name);

    while (fgets(line, MAX_LINE_LEN, f) != NULL) {

        ptr = strtok(line, " ");
        record.arrivalTime = atoi(ptr);

        while (NULL != (ptr = strtok(NULL, " "))) {

            i++;

            switch (i) {
                case 1:
                    record.name = ptr;
                case 2:
                    record.serviceTime = atoi(ptr);
                case 3:
                    record.memoryRequirement = atoi(ptr);
            }
    
        }

        i = 0;
            
        printf("%d, %s, %d, %d\n\n", record.arrivalTime, record.name, record.serviceTime, record.memoryRequirement);

    }

}