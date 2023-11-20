#include <stdlib.h>

#include "string.h"

typedef struct coordinate {
    int x;
    int y;
} coordinate;


// Reads coordinates from a file with `filename`, returns count of lines with coordinates
int read_coordinates(coordinate ***coordinates, const char *filename) {
    FILE *f = fopen(filename, "r");

    string current;
    string_init(&current);
    string_read(&current, f);

    int line_count = atoi(current.values);

    *coordinates = calloc(line_count, sizeof(coordinate*));
    // every line has 3 coordinates
    for (int i = 0; i < line_count; ++i) {
        (*coordinates)[i] = calloc(3, sizeof(coordinate));
    }

    for (int i = 0; i < line_count; ++i) {
        string_read(&current, f);
        int x1 = atoi(current.values);
        string_read(&current, f);
        int y1 = atoi(current.values);
        (*coordinates)[i][0] = (coordinate) { x1, y1 };

        string_read(&current, f);
        int x2 = atoi(current.values);
        string_read(&current, f);
        int y2 = atoi(current.values);
        (*coordinates)[i][1] = (coordinate) { x2, y2 };

        string_read(&current, f);
        int x3 = atoi(current.values);
        string_read(&current, f);
        int y3 = atoi(current.values);
        (*coordinates)[i][2] = (coordinate) { x3, y3 };

        // printf("Got coords: %d, %d; %d, %d; %d, %d\n", (*coordinates)[i][0].x, (*coordinates)[i][0].y,
        //                                                (*coordinates)[i][1].x, (*coordinates)[i][1].y,
        //                                                (*coordinates)[i][2].x, (*coordinates)[i][2].y);
    }

    return line_count;
}