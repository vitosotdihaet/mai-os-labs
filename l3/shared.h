#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 

#include "string.h"


#define crash_on(boolean) if (boolean) { fprintf(stderr, "Error at file %s, line %d\n", __FILE__, __LINE__); exit(1); }
#define handle(i) crash_on(i == -1)


const char* shared_memory_file = "/shared_lab3_mem";

typedef struct {
    string buf;
    sem_t read1;
    sem_t read2;
    sem_t write1;
    sem_t write2;
    int active;
} shared_data;

#endif // SHARED_H