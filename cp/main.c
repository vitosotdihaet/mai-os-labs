#include <stdio.h>
#include <stdlib.h>

#include "binary_allocator.h"
#include "buddy_allocator.h"


void test_buddy() {
    printf("--------------- BUDDY ALLOCATOR BIG TEST ---------------");
    buddy_allocator *buda = buddy_create(1024);

    int *arr1 = (int*) buddy_allocate(buda, sizeof(int));
    arr1[0] = 123;

    printf("\nALLOC'D ARR1: arr1[0] = %d!\n\n", arr1[0]);


    int n = 25;

    char *arr_big = (char*) buddy_allocate(buda, sizeof(char) * n);

    for (int i = 0; i < n; ++i) arr_big[i] = (char) i + 65;

    printf("\nALLOC'D ARR_BIG: arr_big = ");

    for (int i = 0; i < n; ++i) printf("%c, ", arr_big[i]);
    printf("\b\b \b\n\n");


    n = 10;

    int **int_arr = (int**) buddy_allocate(buda, sizeof(int*) * n);
    for (int i = 0; i < n; ++i) {
        int_arr[i] = (int*) buddy_allocate(buda, sizeof(int) * n);
        for (int j = 0; j < n; ++j) {
            int_arr[i][j] = i + j;
        }
    }

    printf("\nALLOC'D INT_ARR: int_arr:\n");

    for (int i = 0; i < n; ++i) {
        printf("%p: ", int_arr[i]);
        for (int j = 0; j < n; ++j) printf("%d, ", int_arr[i][j]);
        printf("\b\b \b\n");
    }
    printf("\b\b \b\n\n");


    buddy_deallocate(buda, arr_big);
    printf("\nDEALLOC'D ARR_BIG!\n\n");
    buddy_print(*buda);

    for (int i = 0; i < n; ++i) {
        printf("\nDEALLOC'D INT_ARR[%d]:\n\n", i);
        buddy_deallocate(buda, int_arr[i]);
        buddy_print(*buda);
    }
    buddy_deallocate(buda, int_arr);
    printf("\nDEALLOC'D INT_ARR:\n\n");
    buddy_print(*buda);

    buddy_deallocate(buda, arr1);
    printf("\nDEALLOC'D ARR1:\n\n");
    buddy_print(*buda);


    buddy_destroy(buda);
    free(buda);


    // printf("\n--------------- BUDDY ALLOCATOR SMOL TEST ---------------\n\n");
    // buddy_allocator *buda2 = buddy_create(8);
    // buddy_print(*buda2);

    // int *new_arr1 = (int*) buddy_allocate(buda2, sizeof(int));
    // new_arr1[0] = 123;

    // printf("\nALLOC'D NEW_ARR1: new_arr1[0] = %d!\n\n", new_arr1[0]);
    // buddy_print(*buda2);


    // int *arr2 = (int*) buddy_allocate(buda2, sizeof(int));

    // printf("\nALLOC'D ARR2: arr2[0] = %d!\n\n", arr2[0]);
    // buddy_print(*buda2);


    // buddy_deallocate(buda2, new_arr1);
    // printf("\nDEALLOC'D ARR1!\n\n");
    // buddy_print(*buda2);

    // buddy_deallocate(buda2, arr2);
    // printf("\nDEALLOC'D ARR2!\n\n");
    // buddy_print(*buda2);


    // buddy_destroy(buda2);
    // free(buda2);
}

void test_bin_alloc() {
    printf("\n--------------- BINARY ALLOCATOR BIG TEST ---------------\n\n");
    binary_allocator *bina = bin_alloc_create_with_block_size(512, 2);

    int *arr1 = (int*) bin_alloc_allocate(bina, sizeof(int));
    arr1[0] = 123;

    printf("\nALLOC'D ARR1: arr1[0] = %d!\n\n", arr1[0]);


    int n = 25;

    char *arr_big = (char*) bin_alloc_allocate(bina, sizeof(char) * n);

    for (int i = 0; i < n; ++i) arr_big[i] = (char) i + 65;

    printf("\nALLOC'D ARR_BIG: arr_big = ");

    for (int i = 0; i < n; ++i) printf("%c, ", arr_big[i]);
    printf("\b\b \b\n\n");


    n = 10;

    int **int_arr = (int**) bin_alloc_allocate(bina, sizeof(int*) * n);
    for (int i = 0; i < n; ++i) {
        int_arr[i] = (int*) bin_alloc_allocate(bina, sizeof(int) * n);
        for (int j = 0; j < n; ++j) {
            int_arr[i][j] = i + j;
        }
    }

    printf("\nALLOC'D INT_ARR: int_arr:\n");

    for (int i = 0; i < n; ++i) {
        printf("%p: ", int_arr[i]);
        for (int j = 0; j < n; ++j) printf("%d, ", int_arr[i][j]);
        printf("\b\b \b\n");
    }
    printf("\b\b \b\n\n");


    bin_alloc_deallocate(bina, arr_big);
    printf("\nDEALLOC'D ARR_BIG:\n\n");
    bin_alloc_print(*bina);

    for (int i = 0; i < n; ++i) bin_alloc_deallocate(bina, int_arr[i]);
    bin_alloc_deallocate(bina, int_arr);
    printf("\nDEALLOC'D INT_ARR:\n\n");
    bin_alloc_print(*bina);

    bin_alloc_deallocate(bina, arr1);
    printf("\nDEALLOC'D ARR1:\n\n");
    bin_alloc_print(*bina);

    bin_alloc_destroy(bina);
    free(bina);


    printf("\n--------------- BINARY ALLOCATOR SMOL TEST ---------------\n\n");
    binary_allocator *bina2 = bin_alloc_create(8);
    bin_alloc_print(*bina2);

    int *new_arr1 = (int*) bin_alloc_allocate(bina2, sizeof(int));
    new_arr1[0] = 123;

    printf("\nALLOC'D NEW_ARR1: new_arr1[0] = %d!\n\n", new_arr1[0]);
    bin_alloc_print(*bina2);


    int *arr2 = (int*) bin_alloc_allocate(bina2, sizeof(int));

    printf("\nALLOC'D ARR2: arr2[0] = %d!\n\n", arr2[0]);
    bin_alloc_print(*bina2);


    bin_alloc_deallocate(bina2, new_arr1);
    printf("\nDEALLOC'D ARR1!\n\n");
    bin_alloc_print(*bina2);

    bin_alloc_deallocate(bina2, arr2);
    printf("\nDEALLOC'D ARR2!\n\n");
    bin_alloc_print(*bina2);


    bin_alloc_destroy(bina2);
    free(bina2);
}


int main() {
    test_buddy();
    // test_bin_alloc();

    return 0;
}