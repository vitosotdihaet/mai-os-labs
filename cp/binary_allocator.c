#include <stdio.h>
#include <assert.h>

#include "binary_allocator.h"


binary_allocator* bin_alloc_create(uint64_t byte_count) {
    uint64_t max_order = closest_n_pow2(byte_count);
    byte_count = pow2(max_order);


    void *memory = (void*) calloc(byte_count, 1);
    assert(memory != NULL);


    block *blocks = (block*) calloc(byte_count, sizeof(block));
    assert(blocks != NULL);

    for (uint64_t i = 0; i < byte_count; ++i) {
        blocks[i].memory = memory + i;
        blocks[i].taken = 0;
    }


    forward_memory **free_blocks = (forward_memory**) calloc(max_order + 1, sizeof(forward_memory*));
    assert(free_blocks != NULL);

    free_blocks[max_order] = calloc(1, sizeof(forward_memory));
    assert(free_blocks[max_order] != NULL);
    free_blocks[max_order]->memory = memory;

    binary_allocator* ba = calloc(sizeof(binary_allocator), 1);

    ba->blocks = blocks;
    ba->free_blocks = free_blocks;
    ba->max_order = max_order;

    return ba;
}

binary_allocator* bin_alloc_create_with_block_size(uint64_t block_count, uint64_t block_size) {
    return bin_alloc_create(block_count * block_size);
}

void bin_alloc_destroy(binary_allocator *ba) {
    free(ba->blocks[0].memory);
    free(ba->blocks);

    for (uint64_t i = 0; i <= ba->max_order; ++i) {
        recursive_free_forward_memory(ba->free_blocks[i]);   
    }
    free(ba->free_blocks);

    free(ba);
}

void* bin_alloc_divide_block(binary_allocator *ba, uint64_t order, uint64_t bytes_needed) {
    if (order == 0 || order > ba->max_order) return NULL;
    void *result = NULL;

    void *memory1 = ba->free_blocks[order]->memory;
    void *memory2 = ba->free_blocks[order]->memory + pow2(order - 1);

    uint64_t block_index = 0;
    for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index].memory == memory1) break;

    forward_memory *next = ba->free_blocks[order]->next;
    free(ba->free_blocks[order]);
    ba->free_blocks[order] = next;

    forward_memory *leftmost_free = ba->free_blocks[order - 1];
    
    forward_memory *new_block2 = calloc(1, sizeof(forward_memory));
    new_block2->memory = memory2;
    new_block2->next = leftmost_free;

    forward_memory *new_block1 = calloc(1, sizeof(forward_memory));
    new_block1->memory = memory1;
    new_block1->next = new_block2;

    ba->free_blocks[order - 1] = new_block1;

    if (pow2(order - 1) > bytes_needed) {        
        result = bin_alloc_divide_block(ba, order - 1, bytes_needed);
    } else {
        result = memory1;

        ba->blocks[block_index].taken = bytes_needed;
        free(ba->free_blocks[order - 1]);
        ba->free_blocks[order - 1] = new_block2;
    }

    return result;
}

void* bin_alloc_allocate(binary_allocator *ba, uint64_t bytes_needed) {
    bytes_needed = closest_pow2(bytes_needed);
    uint64_t order = closest_n_pow2(bytes_needed);

    forward_memory *current_fm = ba->free_blocks[order];

    if (current_fm != NULL) {
        uint64_t block_index = 0;
        for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index].memory == current_fm->memory) break;
        ba->blocks[block_index].taken = bytes_needed;

        forward_memory *next = current_fm->next;
        void *memory = current_fm->memory;

        ba->free_blocks[order] = next;

        free(current_fm);

        return memory;
    }

    while (order <= ba->max_order && ba->free_blocks[order] == NULL) ++order;
    if (order > ba->max_order) return NULL;

    return bin_alloc_divide_block(ba, order, bytes_needed);
}

uint64_t bin_alloc_deallocate(binary_allocator *ba, void *memory) {
    uint64_t block_index = 0;
    for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index].memory == memory) break;

    uint64_t order = closest_n_pow2(ba->blocks[block_index].taken);
    uint64_t result = ba->blocks[block_index].taken;

    ba->blocks[block_index].taken = 0;

    forward_memory *leftmost_free = ba->free_blocks[order];

    forward_memory *new_block = calloc(1, sizeof(forward_memory));
    new_block->memory = memory;
    new_block->next = leftmost_free;
    ba->free_blocks[order] = new_block;

    return result;
}

void bin_alloc_print(binary_allocator ba) {
    printf(
        "ba = {\n"
    );

    if (ba.max_order < 6) {
        printf("\tblocks = {\n\t\t");
        for (uint64_t i = 0; i < pow2(ba.max_order); ++i) {
            printf("%p, %"PRIu64"; ", ba.blocks[i].memory, ba.blocks[i].taken);
        }
        printf("\n\t}\n");
    }


    printf("\tfree_blocks = {\n");
    for (uint64_t i = 0; i <= ba.max_order; ++i) {
        forward_memory *rightmost_free = ba.free_blocks[i];

        printf("\t\t");
        if (rightmost_free == NULL) {
            printf("(nil); ");
        } else {
            while (rightmost_free != NULL) {
                printf("%p; ", rightmost_free->memory);
                rightmost_free = rightmost_free->next;
            }
        }
        printf("\b\b \b\n");
    }
    printf("\t}\n");


    printf("\tmax_order = %"PRIu64"\n", ba.max_order);

    printf("}\n");
}