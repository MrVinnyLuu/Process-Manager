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

    FILE* f = fopen(filepath, "r");
    assert(f);

    process_read(f);

    fclose(f);

    // printf("file: %s\nscheduler: %s\nmemstrat: %s\nquantum: %s\n", filepath, scheduler, memStrat, quantum);

    free(filepath);
    free(scheduler);
    free(memStrat);
    free(quantum);

    return 0;

}
