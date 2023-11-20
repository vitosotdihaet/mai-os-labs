#include <sys/types.h>
#include <unistd.h>

#include <sys/mman.h>

#include "shared.h"


#define handle(i) if (i == -1) { fprintf(stderr, "Error at file %s, line %d\n", __FILE__, __LINE__); exit(1); }
#define crash_on(boolean) if (boolean) { fprintf(stderr, "Error at file %s, line %d\n", __FILE__, __LINE__); exit(1); }



int main() {
    string filename1;
    printf("Input a filename in which you want to write output of child1: ");
    string_read(&filename1);

    string filename2;
    printf("Input a filename in which you want to write output of child2: ");
    string_read(&filename2);


    shared_data* m = mmap(0, sizeof(shared_data), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0); crash_on(m == MAP_FAILED);


    int p1[2];
    handle(pipe(p1));

    pid_t p_id = fork(); handle(p_id);

    if (p_id > 0) { // parent process
        int p2[2];
        handle(pipe(p2));

        p_id = fork(); handle(p_id);

        if (p_id > 0) { // TRUE parent process
            handle(close(p2[0]));
            handle(close(p1[0]));

            string s;
            string_init(&s);

            printf("Input strings [CTRL+D TO EXIT]:\n");
            while (!string_read(&s)) {
                uint64_t l = string_len(s);
                string_push_char(&s, '\n');

                if (l > 10) {
                    if (write(p2[1], s.values, l + 1) < l + 1) {
                        fprintf(stderr, "Write didn't return enough bytes at %s, line %d!\n", __FILE__, __LINE__);
                        exit(1);
                    }
                } else {
                    if (write(p1[1], s.values, l + 1) < l + 1) {
                        fprintf(stderr, "Write didn't return enough bytes at %s, line %d!\n", __FILE__, __LINE__);
                        exit(1);
                    }
                }
            }

            handle(close(p2[1]));
            handle(close(p1[1]));
        } else { // child2
            handle(close(p2[1]));
            handle(dup2(p2[0], STDIN_FILENO));

            char *argv[] = { "processor.out", filename2.values, NULL };
            handle(execv(argv[0], argv));
        }
    } else { // child1
        handle(close(p1[1]));
        handle(dup2(p1[0], STDIN_FILENO));

        char *argv[] = { "processor.out", filename1.values, NULL };
        handle(execv(argv[0], argv));
    }

    return 0;
}