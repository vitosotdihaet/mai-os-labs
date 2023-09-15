#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include "string.h"


int main() {
    string s;
    init_string(&s);

    string filename1;
    printf("Input a filename in which you want to write output of child1: ");
    read_string(&filename1);

    string filename2;
    printf("Input a filename in which you want to write output of child2: ");
    read_string(&filename2);

    int p1[2];
    pipe(p1);

    pid_t p_id = fork();

    if (p_id == -1) {
        printf("Could not fork current process!\n");
        exit(1);
    } else if (p_id > 0) { // parent process
        int p2[2];
        pipe(p2);

        p_id = fork();

        if (p_id == -1) {
            printf("Could not fork current process!\n");
            exit(1);
        } else if (p_id > 0) { // TRUE parent process
            close(p2[0]);
            close(p1[0]);

            sleep(1);

            printf("Input a string: ");
            while (!read_string(&s)) {
                uint64_t l = string_len(s);
                printf("Gotten string = %s, with length of %ld\n", s.values, l);
                push_char(&s, '\n');

                if (l > 10) {
                    write(p2[1], s.values, l + 1);
                } else {
                    write(p1[1], s.values, l + 1);
                }

                printf("Input a string: ");
            }

            close(p2[1]);
            close(p1[1]);
        } else { // child2
            dup2(p2[0], STDIN_FILENO);

            char *argv[] = { "processor.out", filename2.values, NULL };
            execv(argv[0], argv);
        }
    } else { // child1
        dup2(p1[0], STDIN_FILENO);

        char *argv[] = { "processor.out", filename1.values, NULL };
        execv(argv[0], argv);
    }

    return 0;
}