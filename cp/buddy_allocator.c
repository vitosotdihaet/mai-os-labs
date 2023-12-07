#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "buddy_allocator.h"


#define max(a, b) a > b ? a : b


uint64_t closest_pow2(uint64_t n) {
    n -= 1;
    uint64_t result = 1;
    for (; n != 0; n /= 2) result *= 2;
    return result;
}

uint64_t closest_n_pow2(uint64_t n) {
    n -= 1;
    uint64_t result = 0;
    for (; n != 0; n /= 2) result += 1;
    return result;
}

uint64_t pow2(uint64_t n) {
    return 1 << n;
}


buddy_allocator* ba_create(uint64_t byte_count) {
    return ba_create_with_block_size(byte_count, 1);
}

buddy_allocator* ba_create_with_block_size(uint64_t block_count, uint64_t block_size) {
    block_count = closest_pow2(block_count);

    uint64_t byte_count = closest_pow2(block_count * block_size);
    uint64_t max_order = closest_n_pow2(block_count);


    void **memory = (void**) calloc(block_count, sizeof(void*));
    assert(memory != NULL);

    for (uint64_t i = 0; i < block_count; ++i) {
        memory[i] = calloc(1, block_size);
        assert(memory[i] != NULL);
    }


    block **blocks = (block**) calloc(block_count, sizeof(block*));
    assert(blocks != NULL);

    for (uint64_t i = 0; i < block_count; ++i) {
        block *b = calloc(sizeof(block), 1);
        b->memory = memory[i];
        b->taken = 0;
        blocks[i] = b;
        assert(blocks[i] != NULL);
        assert(blocks[i]->memory != NULL);
    }

    free(memory);


    block ***free_blocks = (block***) calloc(max_order + 1, sizeof(block**));
    assert(free_blocks != NULL);

    for (uint64_t i = 0; i <= max_order; ++i) {
        free_blocks[i] = (block**) calloc(byte_count / (block_size * pow2(i)), sizeof(block*));
        assert(free_blocks[i] != NULL);
    }

    free_blocks[max_order][0] = blocks[0];


    buddy_allocator *ba = (buddy_allocator*) calloc(1, sizeof(buddy_allocator));

    ba->blocks = blocks;
    ba->free_blocks = free_blocks;
    ba->block_size = block_size;
    ba->max_order = max_order;

    return ba;
}

void ba_destroy(buddy_allocator *ba) {
    for (uint64_t i = 0; i < pow2(ba->max_order); ++i) {
        free(ba->blocks[i]->memory);
        free(ba->blocks[i]);
    }
    free(ba->blocks);

    for (uint64_t i = 0; i <= ba->max_order; ++i) {
        free(ba->free_blocks[i]);
    }
    free(ba->free_blocks);
}


void* ba_divide_block(buddy_allocator *ba, block *block, uint64_t order, uint64_t bytes_needed) {
    void *result = NULL;

    uint64_t block_index = 0;
    for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index]->memory == block->memory) break;

    uint64_t free_block_moved_index = 0;
    for (uint64_t j = 0; j < pow2(ba->max_order - order); ++j) {
        if (ba->free_blocks[order][j] == NULL) continue;

        if (ba->free_blocks[order][j]->memory == block->memory) {
            ba->free_blocks[order][j] = NULL;

            free_block_moved_index = j * pow2(ba->max_order - order) / 2;

            ba->free_blocks[order - 1][free_block_moved_index] = block; // same as ba->blocks[block_index]
            ba->free_blocks[order - 1][free_block_moved_index + 1] = ba->blocks[block_index + pow2(order - 1)];

            break;
        }
    }

    if (pow2(order - 1) * ba->block_size > bytes_needed) {
        result = ba_divide_block(ba, ba->free_blocks[order - 1][free_block_moved_index], order - 1, bytes_needed);
    } else {
        ba->free_blocks[order - 1][free_block_moved_index]->taken = bytes_needed / ba->block_size;
        result = ba->free_blocks[order - 1][free_block_moved_index]->memory;
        ba->free_blocks[order - 1][free_block_moved_index] = NULL;
    }

    return result;
}

void* ba_allocate(buddy_allocator *ba, uint64_t bytes_needed) {
    void *result = NULL;
    bytes_needed = max(ba->block_size, closest_pow2(bytes_needed));

    for (uint64_t i = 0; i <= ba->max_order; ++i) {
        for (uint64_t j = 0; j < pow2(ba->max_order - i); ++j) {
            if (ba->free_blocks[i][j] == NULL) continue;

            if (pow2(i) * ba->block_size > bytes_needed) {
                return ba_divide_block(ba, ba->free_blocks[i][j], i, bytes_needed);
            } else if (pow2(i) * ba->block_size == bytes_needed) {
                ba->free_blocks[i][j]->taken = bytes_needed / ba->block_size;
                result = ba->free_blocks[i][j]->memory;
                ba->free_blocks[i][j] = NULL;
                return result;
            }
        }
    }

    return result;
}

void* ba_deallocate(buddy_allocator *ba, void *block) {
    uint64_t block_index = 0, free_block_index = 0;
    for (; block_index < pow2(ba->max_order); ++block_index) if (ba->blocks[block_index]->memory == block) break;

    uint64_t order = closest_n_pow2(ba->blocks[block_index]->taken);
    free_block_index = block_index / pow2(order);

    ba->blocks[block_index]->taken = 0;

    ba->free_blocks[order][free_block_index] = ba->blocks[block_index];

    // clamp two blocks into one on higher level
    while (order < ba->max_order) {
        // todo: change to clamp even if it is the right block
        if (free_block_index >= pow2(ba->max_order - order)) break;
        // check if the right block not taken
        if (ba->free_blocks[order][free_block_index + 1] == NULL) break;

        // clamp
        ba->free_blocks[order + 1][free_block_index / 2] = ba->free_blocks[order][free_block_index];
        ba->free_blocks[order][free_block_index] = NULL;
        ba->free_blocks[order][free_block_index + 1] = NULL;

        // go next level
        free_block_index /= 2;
        order++;
    }

    return NULL;
}


void ba_print(buddy_allocator ba) {
    printf(
        "ba = {\n"
        "\tmax_order = %"PRIu64"\n"
        "\tblock_size = %"PRIu64"\n",
        ba.max_order, ba.block_size
    );

    printf("\tblocks = {\n");
    for (uint64_t i = 0; i < pow2(ba.max_order); ++i) {
        printf("\t\t%"PRIu64": %p, %"PRIu64"\n", i, ba.blocks[i]->memory, ba.blocks[i]->taken);
    }
    printf("\t}\n");


    printf("\tfree_blocks: {\n");
    for (uint64_t i = 0; i <= ba.max_order; ++i) {
        printf("\t\t%"PRIu64": ", i);
        for (uint64_t j = 0; j < pow2(ba.max_order - i); ++j) {
            // printf("%"PRIu64": ", j);
            if (ba.free_blocks[i][j] != NULL) {
                printf("%p, %"PRIu64"; ", ba.free_blocks[i][j]->memory, ba.free_blocks[i][j]->taken);
            } else {
                printf("(nil); ");
            }
        }
        printf("\n");
    }
    printf("\t}\n}\n");
}
