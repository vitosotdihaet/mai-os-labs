#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#ifdef __linux__
#include <time.h>
#elif _WIN32
#include <windows.h>
#endif

#include "coordinates.h"

#define DEFAULT_THREADS 1


double max_s = 0;
coordinate max_coords[3];

pthread_mutex_t mutex;

int count;

// Parallel solving problem by solving it on a subarray, comparing to global current max square and storing max_coords in a variable
void *solve(void *args) {
    int *indexes = (int*) args;
    int left_index = indexes[0], right_index = indexes[1];

    coordinate current_max[3];
    double current_max_s = 0;

    double current_s = 0;

    for (int i = left_index; i < right_index; ++i) {
        for (int j = i + 1; j < count - 1; ++j) {
            for (int k = j + 1; k < count; ++k) {
                coordinate a = coordinates[i], b = coordinates[j], c = coordinates[k];
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
        printf("Not enough arguments (one number is needed)\nUsing %d threads as default\n", DEFAULT_THREADS);
    } else {
        num = atoi(argv[1]);

        if (num < 1) {
            printf("Invalid argument!\n");
            num = DEFAULT_THREADS;
        }

        // printf("Using %d threads\n", num);
    }

    return num;
}

int main(int argc, char *argv[]) {
    int threads_num = parse_argv(argc, argv);
    pthread_t *threads = (pthread_t*) calloc(threads_num, sizeof(pthread_t));

    count = sizeof(coordinates)/sizeof(coordinate);

    int index_delta = ceilf((float) count / (float) threads_num);

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Could not initialize mutex!\n");
        return 1;
    }

    int **args = (int**) calloc(threads_num, sizeof(int*));

    for (int i = 0; i < threads_num; ++i) {
        args[i] = calloc(2, sizeof(int));
        args[i][0] = index_delta * i;
        args[i][1] = index_delta * (i + 1);

        if (args[i][0] >= count) { threads_num = i + 1; break; }
        if (args[i][1] > count) { args[i][1] = count; threads_num = i + 1; break; }
    }


#ifdef __linux__
    clock_t time = clock();
#elif _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    double interval;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
#endif


    for (int i = 0; i < threads_num; ++i) {
        int ce = pthread_create(&threads[i], NULL, solve, args[i]);
        if (ce) {
            printf("Couldn't create thread %ld\n", threads[i]);
            exit(ce);
        }
    }

    for (int i = 0; i < threads_num; ++i) {
        int je = pthread_join(threads[i], NULL);
        if (je) {
            printf("Couldn't wait for thread %ld: %d\n", threads[i], je);
            exit(je);
        }
    }


#ifdef __linux__
    time = clock() - time;

    printf("%.32f\n", ((double) time) / CLOCKS_PER_SEC);
    // printf("\nCalculations took %f seconds\n\n", ((double) time) / CLOCKS_PER_SEC);
#elif _WIN32
    QueryPerformanceCounter(&end);
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;

    printf("%.32f\n", interval);
#endif

    // printf("Max square is %f\n", max_s);
    // printf("Coordinates of a triangle with this square are (%d, %d); (%d, %d); (%d, %d)\n",
    //     max_coords[0].x, max_coords[0].y,
    //     max_coords[1].x, max_coords[1].y,
    //     max_coords[2].x, max_coords[2].y
    // );

    for (int i = 0; i < threads_num; ++i) {
        free(args[i]);
    } free(args);

    pthread_mutex_destroy(&mutex);

    return 0;
}