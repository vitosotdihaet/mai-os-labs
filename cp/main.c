#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "binary_allocator.h"
#include "buddy_allocator.h"

#include "test.h"


const uint64_t INITIAL_FREE = 1024;

uint64_t iterations = 1000;

uint64_t free_count = INITIAL_FREE;
uint64_t null_count = 0;
uint64_t alloced_count = 0;


uint64_t random_to(uint64_t to) {
    if (!to) return 0;
    return rand() % to + 1;
}


void random_memory_buddy() {
    forward_block *alloced = calloc(sizeof(forward_block), 1);
    buddy_allocator *b = buddy_create(free_count);

    for (uint64_t i = 0; i < iterations; ++i) {
        printf("iteration = %"PRIu64"\n", i);

        if (rand() > RAND_MAX / 2 && free_count > 0) {
            uint64_t random_to_free = random_to(free_count);
            uint64_t byte_count = closest_pow2(random_to_free);

            void *mem = buddy_allocate(b, random_to_free);

            if (mem == NULL) {
                null_count++;
            } else {
                alloced_count++;
                free_count -= byte_count;
                forward_block *next = alloced->next;

                alloced->next = calloc(sizeof(forward_block), 1);
                alloced->next->memory = mem;
                alloced->next->next = next;
            }
        } else if (alloced_count > 0) {
            forward_block *current = alloced, *previous = NULL;
            for (uint64_t j = 0; j < random_to(alloced_count); ++j) {
                previous = current;
                current = current->next;
            }

            forward_block *next = current->next;
            uint64_t byte_count = buddy_deallocate(b, current->memory);
            free_count += byte_count;
            free(current);
            --alloced_count;

            previous->next = next;
        }
    } 
 
    buddy_destroy(b);
}

void random_memory_binary() {
    forward_block *alloced = calloc(sizeof(forward_block), 1);
    binary_allocator *b = bin_alloc_create(free_count);

    for (uint64_t i = 0; i < iterations; ++i) {
        printf("iteration = %"PRIu64"\n", i);

        if (rand() > RAND_MAX / 2 && free_count > 0) {
            uint64_t random_to_free = random_to(free_count);
            uint64_t byte_count = closest_pow2(random_to_free);

            void *mem = bin_alloc_allocate(b, random_to_free);

            if (mem == NULL) {
                null_count++;
            } else {
                alloced_count++;
                free_count -= byte_count;
                forward_block *next = alloced->next;

                alloced->next = calloc(sizeof(forward_block), 1);
                alloced->memory = mem;
                alloced->next->next = next;
            }
        } else if (alloced_count > 0) {
            forward_block *current = alloced, *previous = NULL;
            for (uint64_t j = 0; j < random_to(alloced_count); ++j) {
                previous = current;
                current = current->next;
            }

            forward_block *next = current->next;
            uint64_t byte_count = bin_alloc_deallocate(b, current->memory);
            free_count += byte_count;
            free(current);
            --alloced_count;

            previous->next = next;
        }
    }

    for (uint64_t i = 0; i < alloced_count; ++i) {
        // free
    }
    bin_alloc_destroy(b);
}



int main() {
    // test_buddy();
    // test_bin_alloc();

    // random_memory_buddy();
    // printf(
    //     "--- BUDDY RESULTS ---\n"
    //     "free_count = %"PRIu64"\n"
    //     "null_count = %"PRIu64"\n"
    //     "alloced_count = %"PRIu64"\n\n",
    //     free_count, null_count, alloced_count
    // );

    // free_count = INITIAL_FREE;
    // null_count = 0;
    // alloced_count = 0;

    random_memory_binary();
    printf(
        "--- BIN RESULTS ---\n"
        "free_count = %"PRIu64"\n"
        "null_count = %"PRIu64"\n"
        "alloced_count = %"PRIu64"\n",
        free_count, null_count, alloced_count
    );

    return 0;
}