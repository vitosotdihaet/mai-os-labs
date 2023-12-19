#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "binary_allocator.h"


binary_allocator* bin_alloc_create(uint64_t byte_count) {
    byte_count = closest_pow2(byte_count);

    void *memory = calloc(byte_count, 1);
    assert(memory != NULL);

    block *blocks = calloc(byte_count, sizeof(block));
    assert(blocks != NULL);

    for (uint64_t i = 0; i < byte_count; ++i) {
        blocks[i].memory = memory + i;
    }

    uint64_t *free = calloc(byte_count, sizeof(uint64_t));
    for (uint64_t i = 0; i < byte_count; ++i) {
        free[i] = byte_count - i;
    }

    binary_allocator* ba = calloc(sizeof(binary_allocator), 1);

    ba->blocks = blocks;
    ba->free = free;
    ba->byte_count = byte_count;
}

binary_allocator* bin_alloc_create_with_block_size(uint64_t block_count, uint64_t block_size) {
    return bin_alloc_create(block_count * block_size);
}

void bin_alloc_destroy(binary_allocator *ba) {
    free(ba->blocks[0].memory);
    free(ba->blocks);
    free(ba->free);
}

void* bin_alloc_allocate(binary_allocator *ba, uint64_t bytes_needed) {
    void *result = NULL;
    bytes_needed = closest_pow2(bytes_needed);

    for (uint64_t i = 0; i < ba->byte_count; ++i) {
        if (ba->free[i] < bytes_needed) {
            continue;
        }

        result = ba->blocks[i].memory;
        for (uint64_t j = 0; j < bytes_needed; ++j) {
            ba->free[i + j] = 0;
            ba->blocks[i + j].taken = bytes_needed - j;
        }

        break;
    }

    return result;
}

void* bin_alloc_deallocate(binary_allocator *ba, void *block) {
    for (uint64_t i = 0; i < ba->byte_count; ++i) {
        if (block != ba->blocks[i].memory) continue;

        uint64_t bytes_taken = ba->blocks[i].taken;
        for (uint64_t j = 0; j < bytes_taken; ++j) {
            ba->blocks[i + j].taken = 0;
            ba->free[i + j] = bytes_taken - j;
        }

        // check if there are free bytes after deallocated
        if (i + bytes_taken < ba->byte_count && ba->free[i + bytes_taken] != 0) {
            for (uint64_t j = bytes_taken; j > 1; --j) {
                ba->free[i + j - 1] = ba->free[i + j] + 1;
            }
        }

        // check if there are were free bytes before deallocated
        ++i;
        for (; i > 1 && ba->free[i - 1 - 1] != 0; --i) {
            ba->free[i - 1 - 1] = ba->free[i - 1] + 1;
        }

        break;
    }

    return NULL;
}

void bin_alloc_print(binary_allocator ba) {
    printf(
        "ba = {\n"
        // "\tblocks = {\n\t\t"
    );

    // for (uint64_t i = 0; i < ba.byte_count; ++i) {
    //     printf("(%p, %"PRIu64"), ", ba.blocks[i].memory, ba.blocks[i].taken);
    // }
    // printf("\b\b \b\n\t}\n");


    printf("\tfree = {\n\t\t");
    for (uint64_t i = 0; i < ba.byte_count; ++i) {
        printf("%"PRIu64", ", ba.free[i]);
    }
    printf("\b\b \b\n\t}\n");


    printf("\tbyte_count = %"PRIu64"\n", ba.byte_count);

    printf("}\n");
}