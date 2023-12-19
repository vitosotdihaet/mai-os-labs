#pragma once

#include <inttypes.h>


#define max(a, b) a > b ? a : b


typedef struct {
    void *memory;
    uint64_t taken;
} block;


uint64_t closest_pow2(uint64_t n);
uint64_t closest_n_pow2(uint64_t n);
uint64_t pow2(uint64_t n);