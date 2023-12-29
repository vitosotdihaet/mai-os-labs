#include <stdio.h>
#include <assert.h>

#include "buddy_allocator.h"


buddy_allocator* buddy_create(uint64_t byte_count) {
    byte_count = closest_pow2(byte_count);
    uint64_t max_order = closest_n_pow2(byte_count);

    void *memory = (void*) calloc(byte_count, 1);
    assert(memory != NULL);

    forward_memory **free_blocks = (forward_memory**) calloc(max_order + 1, sizeof(forward_memory*));
    assert(free_blocks != NULL);

    free_blocks[max_order] = (forward_memory*) calloc(1, sizeof(forward_memory));
    free_blocks[max_order]->memory = memory;

    buddy_allocator *ba = (buddy_allocator*) calloc(1, sizeof(buddy_allocator));
    assert(ba != NULL);

    ba->free_blocks = free_blocks;
    ba->max_order = max_order;
    ba->mem_start = memory;

    return ba;
}

buddy_allocator* buddy_create_with_block_size(uint64_t block_count, uint64_t block_size) {
    return buddy_create(block_count * block_size);
}

void buddy_destroy(buddy_allocator *ba) {
    for (uint64_t i = 0; i <= ba->max_order; ++i) {
        recursive_free_forward_memory(ba->free_blocks[i]);
    }
    free(ba->free_blocks);
    free(ba->mem_start);
    free(ba);
}

void* buddy_divide_block(buddy_allocator *ba, uint64_t order, uint64_t order_needed) {
    void *result = NULL;
    uint64_t shift = pow2(order - 1);

    // remove from current order
    void *memory = ba->free_blocks[order]->memory;
    forward_memory *next = ba->free_blocks[order]->next;
    free(ba->free_blocks[order]);

    ba->free_blocks[order] = next;
    
    // add its buddy to order - 1
    forward_memory *right_buddy = calloc(1, sizeof(forward_memory));
    right_buddy->memory = memory + shift;
    right_buddy->next = ba->free_blocks[order - 1];
    ba->free_blocks[order - 1] = right_buddy;

    // add left block to order - 1 and divide
    if (order - 1 != order_needed) {
        forward_memory *left_buddy = calloc(1, sizeof(forward_memory));
        left_buddy->memory = memory;
        left_buddy->next = ba->free_blocks[order - 1];
        ba->free_blocks[order - 1] = left_buddy;

        result = buddy_divide_block(ba, order - 1, order_needed);
    } else {
        result = memory;
    }

    return result;
}

void* buddy_allocate(buddy_allocator *ba, uint64_t bytes_needed) {
    uint64_t order_needed = closest_n_pow2(bytes_needed), order = order_needed;
    if (order > ba->max_order) return NULL;

    forward_memory *current_fm = ba->free_blocks[order];

    if (current_fm != NULL) {
        forward_memory *next = current_fm->next;
        void *memory = current_fm->memory;

        ba->free_blocks[order] = next;

        free(current_fm);

        return memory;
    }

    while (order <= ba->max_order && ba->free_blocks[order] == NULL) ++order;
    if (order > ba->max_order) return NULL;

    return buddy_divide_block(ba, order, order_needed);
}

void buddy_merge(buddy_allocator *ba, uint64_t order) {
    void *memory = ba->free_blocks[order]->memory;
    uint64_t byte_count = pow2(order);

    int is_right = ((memory - ba->mem_start) / byte_count) % 2;

    forward_memory *buddy_left = NULL;
    forward_memory *buddy_right = NULL;

    void *buddy_memory = NULL;

    if (is_right) {
        // right one is the first in free_blocks
        buddy_right = ba->free_blocks[order];
        buddy_memory = memory - byte_count;
    } else {
        // right one is the first in free_blocks
        buddy_left = ba->free_blocks[order];
        buddy_memory = memory + byte_count;
    }

    // for linking the gap created by merging
    forward_memory *previous = NULL;
    forward_memory *current = ba->free_blocks[order]->next;

    // search for the buddy block
    while (current != NULL) {
        if (current->memory == buddy_memory) {
            if (is_right) {
                buddy_left = current;
            } else {
                buddy_right = current;
            }
            break;
        }

        previous = current;
        current = current->next;
    }

    if (buddy_left && buddy_right) {
        // merge higher
        forward_memory *merged = (forward_memory*) calloc(1, sizeof(forward_memory));
        merged->memory = buddy_left->memory;

        // remove initial buddy
        if (is_right) {
            ba->free_blocks[order] = buddy_right->next;
        } else {
            ba->free_blocks[order] = buddy_left->next;
        }

        // remove found buddy
        if (previous != NULL) {
            if (is_right) {
                previous->next = buddy_left->next;
            } else {
                previous->next = buddy_right->next;
            }
        } else {
            // the first element was a found buddy
            if (is_right) {
                ba->free_blocks[order] = buddy_left->next;
            } else {
                ba->free_blocks[order] = buddy_right->next;
            }
        }

        // free both because they should not be present in free_blocks anymore
        free(buddy_right);
        free(buddy_left);

        // put merged block to a higher order
        merged->next = ba->free_blocks[order + 1];
        ba->free_blocks[order + 1] = merged;

        buddy_merge(ba, order + 1);
    }
}

uint64_t buddy_deallocate(buddy_allocator *ba, void *memory, uint64_t byte_count) {
    byte_count = closest_pow2(byte_count);

    uint64_t order = closest_n_pow2(byte_count);
    if (order >= ba->max_order) return 0;

    // is this memory from a right buddy block?
    int is_right = ((memory - ba->mem_start) / byte_count) % 2;

    forward_memory *buddy_left = NULL;
    forward_memory *buddy_right = NULL;

    void *buddy_memory = NULL;

    if (is_right) {
        // right one is deallocated so we create it to put in free_blocks
        buddy_right = (forward_memory*) calloc(1, sizeof(forward_memory));
        buddy_right->memory = memory;
        buddy_memory = memory - byte_count;
    } else {
        // left one is deallocated so we create it to put in free_blocks
        buddy_left = (forward_memory*) calloc(1, sizeof(forward_memory));
        buddy_left->memory = memory;
        buddy_memory = memory + byte_count;
    }

    // for linking the gap created by merging
    forward_memory *previous = NULL;
    forward_memory *current = ba->free_blocks[order];

    // search for the buddy block
    while (current != NULL) {
        if (current->memory == buddy_memory) {
            if (is_right) {
                buddy_left = current;
            } else {
                buddy_right = current;
            }
            break;
        }

        previous = current;
        current = current->next;
    }

    if (buddy_left && buddy_right) {
        // merge higher
        forward_memory *merged = (forward_memory*) calloc(1, sizeof(forward_memory));
        merged->memory = buddy_left->memory;

        // remove found buddy
        if (previous != NULL) {
            if (is_right) {
                previous->next = buddy_left->next;
            } else {
                previous->next = buddy_right->next;
            }
        } else {
            // the first element was a found buddy
            if (is_right) {
                ba->free_blocks[order] = buddy_left->next;
            } else {
                ba->free_blocks[order] = buddy_right->next;
            }
        }

        // free both because they should not be present in free_blocks anymore
        free(buddy_right);
        free(buddy_left);

        // put merged block to a higher order
        merged->next = ba->free_blocks[order + 1];
        ba->free_blocks[order + 1] = merged;

        buddy_merge(ba, order + 1);
    } else {
        // put deallocated block to free_blocks
        forward_memory *leftmost_fm = NULL;
        if (is_right) {
            leftmost_fm = buddy_right;
        } else {
            leftmost_fm = buddy_left;
        }

        leftmost_fm->next = ba->free_blocks[order];
        ba->free_blocks[order] = leftmost_fm;
    }

    return byte_count;
}


void buddy_print(buddy_allocator ba) {
    printf(
        "ba = {\n"
    );

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
    printf("\tmem_start = %p\n", ba.mem_start);

    printf("}\n");
}
