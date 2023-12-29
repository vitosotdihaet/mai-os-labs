#ifndef _BUDDY_ALLOCATOR_H
#define _BUDDY_ALLOCATOR_H

#include "shared.h"


typedef struct {
    forward_memory **free_blocks;
    uint64_t max_order;
    void *mem_start;
} buddy_allocator;


buddy_allocator* buddy_create(uint64_t byte_count);
buddy_allocator* buddy_create_with_block_size(uint64_t block_count, uint64_t block_size);
void buddy_destroy(buddy_allocator *ba);

void* buddy_allocate(buddy_allocator *ba, uint64_t bytes_needed);
uint64_t buddy_deallocate(buddy_allocator *ba, void *memory, uint64_t byte_count);
void buddy_print(buddy_allocator ba);


#endif // _BUDDY_ALLOCATOR_H
