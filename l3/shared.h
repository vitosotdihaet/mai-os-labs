#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h>

#include "string.h"


const char* shm_name = "/my_shared_memory";

typedef struct {
    string buf;
    sem_t semaphore_lock;
} shared_data;
