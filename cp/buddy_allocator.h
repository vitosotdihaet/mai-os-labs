#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <inttypes.h>

typedef struct {
    void *memory;
    uint64_t taken;
} block;

typedef struct {
    // all the blocks of memory
    block **blocks;
    // ith element of free_blocks is an array of blocks of order i
    block ***free_blocks;
    uint64_t max_order;
} buddy_allocator;

buddy_allocator* ba_create(uint64_t byte_count);
buddy_allocator* ba_create_with_block_size(uint64_t block_count, uint64_t block_size);
void ba_destroy(buddy_allocator *ba);

void* ba_allocate(buddy_allocator *ba, uint64_t byte_count);
void* ba_deallocate(buddy_allocator *ba, void *block);
void ba_print(buddy_allocator ba);


#endif // BUDDY_ALLOCATOR_H
