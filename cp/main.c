#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "binary_allocator.h"
#include "buddy_allocator.h"

#include "test.h"


const uint64_t INITIAL_FREE = 1024 * 1024;

uint64_t iterations = 1000;

uint64_t buddy_null_count = 0;
uint64_t buddy_alloced_count = 0;
uint64_t buddy_dealloced_count = 0;

uint64_t bin_alloc_null_count = 0;
uint64_t bin_alloc_alloced_count = 0;
uint64_t bin_alloc_dealloced_count = 0;


uint64_t random_to(uint64_t to) {
    return rand() % to + 1;
}

typedef struct ft {
    void *memory;
    uint64_t bytes;
    struct ft *next;
} forward_taken;


void recursive_free_forward_memory(forward_memory *block) {
    if (block == NULL) return;

    recursive_free_forward_memory(block->next);
    free(block);
}


void free_forward_memory(forward_memory *block) {
    if (block == NULL) return;

    free_forward_memory(block->next);
    free(block);
}

void free_forward_taken(forward_taken *block) {
    if (block == NULL) return;

    free_forward_taken(block->next);
    free(block);
}


void random_memory_buddy(int print) {
    uint64_t free_count = INITIAL_FREE;
    uint64_t current_alloced = 0;

    forward_taken *alloced = calloc(sizeof(forward_taken), 1);
    buddy_allocator *b = buddy_create(free_count);

    for (uint64_t i = 0; i < iterations; ++i) {
        if (print && i % (iterations / 10) == 0) printf("i = %"PRIu64"\n", i);

        if (current_alloced == 0 || (rand() > RAND_MAX / 2 && free_count > 0)) {
            uint64_t byte_count = pow2(random_to(closest_n_pow2(free_count / 2)));

            void *mem = buddy_allocate(b, byte_count);

            if (mem == NULL) {
                buddy_null_count++;
            } else {
                buddy_alloced_count++;
                current_alloced++;
                free_count -= byte_count;

                forward_taken *after = alloced->next;
                forward_taken *next = calloc(sizeof(forward_taken), 1);

                next->memory = mem;
                next->bytes = byte_count;
                next->next = after;

                alloced->next = next;
            }
        } else if (current_alloced > 0) {
            buddy_dealloced_count++;
            forward_taken *current = alloced, *previous = NULL;

            for (uint64_t j = 0; j < random_to(current_alloced); ++j) {
                previous = current;
                current = current->next;
            }

            forward_taken *next = current->next;
            uint64_t byte_count = buddy_deallocate(b, current->memory, current->bytes);
            free_count += byte_count;
            free(current);
            --current_alloced;

            previous->next = next;
        }
    }

    free_forward_taken(alloced);

    buddy_destroy(b);
}


void random_memory_binary(int print) {
    uint64_t free_count = INITIAL_FREE;
    uint64_t current_alloced = 0;

    forward_memory *alloced = calloc(sizeof(forward_memory), 1);
    binary_allocator *b = bin_alloc_create(free_count);

    for (uint64_t i = 0; i < iterations; ++i) {
        if (print && i % (iterations / 10) == 0) printf("i = %"PRIu64"\n", i);

        if (current_alloced == 0 || (rand() > RAND_MAX / 2 && free_count > 0)) {
            uint64_t byte_count = pow2(random_to(closest_n_pow2(free_count / 2)));

            void *mem = bin_alloc_allocate(b, byte_count);

            if (mem == NULL) {
                bin_alloc_null_count++;
            } else {
                bin_alloc_alloced_count++;
                current_alloced++;
                free_count -= byte_count;

                forward_memory *after = alloced->next;
                forward_memory *next = calloc(sizeof(forward_memory), 1);

                next->memory = mem;
                next->next = after;

                alloced->next = next;
            }
        } else if (current_alloced > 0) {
            bin_alloc_dealloced_count++;
            forward_memory *current = alloced, *previous = NULL;

            for (uint64_t j = 0; j < random_to(current_alloced); ++j) {
                previous = current;
                current = current->next;
            }

            forward_memory *next = current->next;
            uint64_t byte_count = bin_alloc_deallocate(b, current->memory);
            free_count += byte_count;
            free(current);
            --current_alloced;

            previous->next = next;
        }
    }

    free_forward_memory(alloced);

    bin_alloc_destroy(b);
}


void test_same_seed(int test_count) {
    for (int i = 0; i < test_count; ++i) {
        // printf("TEST #%d.1\n\n", i + 1);
        srand(i);

        random_memory_buddy(0);
        // printf(
        //     "--- BUDDY RESULTS ---\n"
        //     "null_count = %"PRIu64"\n"
        //     "alloced_count = %"PRIu64"\n"
        //     "dealloced_count = %"PRIu64"\n\n",
        //     null_count, alloced_count, dealloced_count
        // );

        random_memory_binary(0);
        // printf(
        //     "--- BIN RESULTS ---\n"
        //     "null_count = %"PRIu64"\n"
        //     "alloced_count = %"PRIu64"\n"
        //     "dealloced_count = %"PRIu64"\n\n",
        //     null_count, alloced_count, dealloced_count
        // );

        // printf("TEST #%d.2\n\n", i + 1);
        srand(i);

        random_memory_binary(0);
        // printf(
        //     "--- BIN RESULTS ---\n"
        //     "null_count = %"PRIu64"\n"
        //     "alloced_count = %"PRIu64"\n"
        //     "dealloced_count = %"PRIu64"\n\n",
        //     null_count, alloced_count, dealloced_count
        // );

        random_memory_buddy(0);
        // printf(
        //     "--- BUDDY RESULTS ---\n"
        //     "null_count = %"PRIu64"\n"
        //     "alloced_count = %"PRIu64"\n"
        //     "dealloced_count = %"PRIu64"\n\n",
        //     null_count, alloced_count, dealloced_count
        // );
    }

    printf(
        "GLOBALS:\n"
        "--- BUDDY RESULTS ---\n"
        "null_count = %"PRIu64"\n"
        "alloced_count = %"PRIu64"\n"
        "dealloced_count = %"PRIu64"\n\n"
        "--- BIN RESULTS ---\n"
        "null_count = %"PRIu64"\n"
        "alloced_count = %"PRIu64"\n"
        "dealloced_count = %"PRIu64"\n\n",

        buddy_null_count, buddy_alloced_count, buddy_dealloced_count,
        bin_alloc_null_count, bin_alloc_alloced_count, bin_alloc_dealloced_count
    );
}


int main() {
    srand(time(NULL));

    test_same_seed(10);

    return 0;
}