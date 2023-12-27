#include <stdio.h>
#include <stdlib.h>
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


    forward_block **free_blocks = (forward_block**) calloc(max_order + 1, sizeof(forward_block*));
    assert(free_blocks != NULL);

    free_blocks[max_order] = calloc(1, sizeof(forward_block));
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

void recursive_free_forward_block(forward_block *block) {
    if (block == NULL) return;

    recursive_free_forward_block(block->next);
    free(block);
}

void bin_alloc_destroy(binary_allocator *ba) {
    free(ba->blocks[0].memory);
    free(ba->blocks);

    for (uint64_t i = ba->max_order + 1; i > 1; --i) {
        recursive_free_forward_block(ba->free_blocks[i - 1]);   
    }
    free(ba->free_blocks);
}

void* bin_alloc_divide_block(binary_allocator *ba, uint64_t order, uint64_t bytes_needed) {
    void *result = NULL;

    void *memory1 = ba->free_blocks[order]->memory;
    void *memory2 = ba->free_blocks[order]->memory + pow2(order - 1);

    uint64_t block_index = 0;
    for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index].memory == memory1) break;

    forward_block *next = ba->free_blocks[order]->next;
    free(ba->free_blocks[order]);
    ba->free_blocks[order] = next;

    forward_block *leftmost_free = ba->free_blocks[order - 1];
    
    forward_block *new_block2 = calloc(1, sizeof(forward_block));
    new_block2->memory = memory2;
    new_block2->next = leftmost_free;

    forward_block *new_block1 = calloc(1, sizeof(forward_block));
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
    void *result = NULL;
    bytes_needed = closest_pow2(bytes_needed);

    for (uint64_t i = 0; i <= ba->max_order; ++i) {
        if (ba->free_blocks[i] == NULL) continue;

        if (pow2(i) > bytes_needed) {
            return bin_alloc_divide_block(ba, i, bytes_needed);
        } else if (pow2(i) == bytes_needed) {
            result = ba->free_blocks[i]->memory;

            uint64_t block_index = 0;
            for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index].memory == result) break;
            ba->blocks[block_index].taken = bytes_needed;

            forward_block *next = ba->free_blocks[i]->next;
            free(ba->free_blocks[i]);
            ba->free_blocks[i] = next;

            return result;
        }
    }

    return result;
}

void* bin_alloc_deallocate(binary_allocator *ba, void *memory) {
    uint64_t block_index = 0;
    for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index].memory == memory) break;

    uint64_t order = closest_n_pow2(ba->blocks[block_index].taken);
    ba->blocks[block_index].taken = 0;

    forward_block *leftmost_free = ba->free_blocks[order];
    
    forward_block *new_block = calloc(1, sizeof(forward_block));
    new_block->memory = memory;
    new_block->next = leftmost_free;

    ba->free_blocks[order] = new_block;

    return NULL;
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
        forward_block *rightmost_free = ba.free_blocks[i];

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