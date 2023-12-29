#pragma once

#include <stdlib.h>
#include <inttypes.h>


#define max(a, b) a > b ? a : b


typedef struct {
    void *memory;
    uint64_t taken;
} block;

typedef struct fm {
    void *memory;
    struct fm *next;
} forward_memory;


uint64_t closest_pow2(uint64_t n);
uint64_t closest_n_pow2(uint64_t n);
uint64_t pow2(uint64_t n);