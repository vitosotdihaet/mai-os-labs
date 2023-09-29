#include <stdio.h>
#include <pthread.h>

int parse_argv(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Not enough arguments (one is needed)\nUsing 1 thread as default");
        return 1;
    } else {
        int num = atoi(argv[2]);
        printf("Got one argument = %d\nUsing %d threads", num, num);
        return num;
    }
}

int main(int argc, char *argv[]) {
    int threads_num = parse_argv(argc, argv);

    // ...

    return 0;
}