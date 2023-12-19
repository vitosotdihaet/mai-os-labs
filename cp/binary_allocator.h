#ifndef _BINARY_ALLOCATOR_H
#define _BINARY_ALLOCATOR_H

#include "shared.h"
#include <stdbool.h>


typedef struct fb {
    void *memory;
    struct fb *next;
} forward_block;

typedef struct {
    block *blocks;
    forward_block **free_blocks; // ith element is a forward-list of memory with capacity 2^i
    uint64_t max_order;
} binary_allocator;

/*
free_blocks:
capacity = 2^0: [*, ->][*, ->]0
capacity = 2^1: [*, ->]0
*/


binary_allocator* bin_alloc_create(uint64_t byte_count);
binary_allocator* bin_alloc_create_with_block_size(uint64_t block_count, uint64_t block_size);
void bin_alloc_destroy(binary_allocator *ba);

void* bin_alloc_allocate(binary_allocator *ba, uint64_t byte_count);
void* bin_alloc_deallocate(binary_allocator *ba, void *block);

void bin_alloc_print(binary_allocator ba);

#endif // _BINARY_ALLOCATOR_H