#include <stdio.h>
#include <stdlib.h>

#include "binary_allocator.h"
#include "buddy_allocator.h"


int main() {
    buddy_allocator *buda = ba_create_with_block_size(1000, sizeof(char));

    int *arr1 = (int*) ba_allocate(buda, sizeof(int));
    arr1[0] = 123;

    printf("arr1[0] = %d\n", arr1[0]);

    int n = 200;

    char *arr_big = (char*) ba_allocate(buda, sizeof(char) * n);
    // ba_print(*buda);

    for (int i = 0; i < n; ++i) arr_big[i] = (char) i + 65;
    for (int i = 0; i < n; ++i) printf("%c, ", arr_big[i]);
    printf("\b\b \b\n");

    ba_deallocate(buda, arr_big);
    // ba_print(*buda);

    ba_destroy(buda);
    free(buda);

    return 0;
}