#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "coordinates.h"

#define DEFAULT_THREADS 1


double max_s = 0;
coordinate max_coords[3];

// int *checked;

pthread_mutex_t mutex;

// Parallel solving problem by solving it on a subarray, comparing to global current max square and storing max_coords in a variable
void *solve(void *args) {
    int *indexes = (int*) args;
    int left_index = indexes[0], right_index = indexes[1];
    free(args);

    coordinate current_max[3];
    double current_max_s = 0;

    double current_s = 0;

    for (int i = left_index; i < right_index; ++i) {
        // checked[i]++;
        coordinate a = coordinates[i][0], b = coordinates[i][1], c = coordinates[i][2];
        int x1 = a.x, y1 = a.y;
        int x2 = b.x, y2 = b.y;
        int x3 = c.x, y3 = c.y;

        current_s = 0.5 * abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1));

        if (current_s > current_max_s) {
            current_max_s = current_s;
            current_max[0] = a;
            current_max[1] = b;
            current_max[2] = c;
        }
    }

    pthread_mutex_lock(&mutex);
    if (current_max_s > max_s) {
        max_s = current_max_s;
        max_coords[0] = current_max[0];
        max_coords[1] = current_max[1];
        max_coords[2] = current_max[2];
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}


int parse_argv(int argc, char *argv[]) {
    int num = DEFAULT_THREADS;

    if (argc != 2) {
        // printf("Not enough arguments (one number is needed)\nUsing %d threads as default\n", DEFAULT_THREADS);
    } else {
        num = atoi(argv[1]);

        if (num < 1) {
            // printf("Invalid argument!\n");
            num = DEFAULT_THREADS;
        }

        // printf("Using %d threads\n", num);
    }

    return num;
}

int main(int argc, char *argv[]) {
    int threads_num = parse_argv(argc, argv);
    pthread_t *threads = (pthread_t*) calloc(threads_num, sizeof(pthread_t));

    int count = sizeof(coordinates)/(sizeof(coordinate) * 3);

    // checked = calloc(count, sizeof(int));

    int index_delta = ceilf((float) count/ (float) threads_num);

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Could not initialize mutex!\n");
        return 1;
    }

    for (int i = 0; i < threads_num; ++i) {
        int *arg = calloc(2, sizeof(int));
        arg[0] = index_delta * i;
        arg[1] = index_delta * (i + 1);

        if (arg[0] >= count) break;
        if (arg[1] > count) arg[1] = count;

        int ce = pthread_create(&threads[i], NULL, solve, arg);
        if (ce) {
            printf("Couldn't create thread %ld\n", threads[i]);
            exit(ce);
        }
    }


    clock_t time = clock();

    for (int i = 0; i < threads_num; ++i) {
        int je = pthread_join(threads[i], NULL);
        if (je) {
            printf("Couldn't start thread %ld\n", threads[i]);
            exit(je);
        }
    }

    time = clock() - time;


    printf("%.32f\n", ((double) time) / CLOCKS_PER_SEC);

    // printf("\nCalculations took %f seconds\n\n", ((double) time) / CLOCKS_PER_SEC);

    // printf("Max square is %f\n", max_s);
    // printf("Coordinates of a triangle with this square are (%d, %d); (%d, %d); (%d, %d)\n",
    //     max_coords[0].x, max_coords[0].y,
    //     max_coords[1].x, max_coords[1].y,
    //     max_coords[2].x, max_coords[2].y
    // );

    pthread_mutex_destroy(&mutex);

    // for (int i = 0; i < count; ++i) {
    //     if (checked[i] > 0) printf("checked[%d] = %d\n", i, checked[i]);
    // }

    return 0;
}