#include <stdio.h>
#include <stdlib.h>

#include "binary_allocator.h"
#include "buddy_allocator.h"


int main() {
    buddy_allocator *buda = ba_create(1024);

    int *arr1 = (int*) ba_allocate(buda, sizeof(int));
    arr1[0] = 123;

    printf("\nALLOC'D ARR1: arr1[0] = %d!\n\n", arr1[0]);


    int n = 25;

    char *arr_big = (char*) ba_allocate(buda, sizeof(char) * n);

    for (int i = 0; i < n; ++i) arr_big[i] = (char) i + 65;

    printf("\nALLOC'D ARR_BIG: arr_big = ");

    for (int i = 0; i < n; ++i) printf("%c, ", arr_big[i]);
    printf("\b\b \b\n\n");


    n = 10;

    int **int_arr = (int**) ba_allocate(buda, sizeof(int*) * n);
    printf("int_arr = %p\n", int_arr);
    for (int i = 0; i < n; ++i) {
        int_arr[i] = (int*) ba_allocate(buda, sizeof(int) * n);
        printf("int_arr[%d] = %p\n", i, int_arr[i]);
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


    ba_deallocate(buda, arr_big);
    ba_destroy(buda);
    free(buda);


    buddy_allocator *buda2 = ba_create(8);

    int *new_arr1 = (int*) ba_allocate(buda2, sizeof(int));
    new_arr1[0] = 123;

    printf("\nALLOC'D NEW_ARR1: new_arr1[0] = %d!\n\n", new_arr1[0]);
    ba_print(*buda2);


    int *arr2 = (int*) ba_allocate(buda2, sizeof(int));

    printf("\nALLOC'D ARR2: arr2[0] = %d!\n\n", arr2[0]);
    ba_print(*buda2);


    ba_deallocate(buda2, new_arr1);
    printf("\nDEALLOC'D ARR1!\n\n");
    ba_print(*buda2);

    ba_deallocate(buda2, arr2);
    printf("\nDEALLOC'D ARR2!\n\n");
    ba_print(*buda2);


    ba_destroy(buda2);
    free(buda2);

    return 0;
}