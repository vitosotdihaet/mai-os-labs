#ifndef _BINARY_ALLOCATOR_H
#define _BINARY_ALLOCATOR_H

#include "shared.h"


typedef struct {
    block *blocks; // [*, 2], [*, 1], [*, 0], [*, 1], [*, 0], ...
    uint64_t *free; // 0, 0, 1, 0, 1, ...
    uint64_t byte_count;
} binary_allocator;


binary_allocator* bin_alloc_create(uint64_t byte_count);
binary_allocator* bin_alloc_create_with_block_size(uint64_t block_count, uint64_t block_size);
void bin_alloc_destroy(binary_allocator *ba);

void* bin_alloc_allocate(binary_allocator *ba, uint64_t byte_count);
void* bin_alloc_deallocate(binary_allocator *ba, void *block);

void bin_alloc_print(binary_allocator ba);

#endif // _BINARY_ALLOCATOR_H