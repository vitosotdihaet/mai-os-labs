#include <sys/types.h>
#include <unistd.h>

#include "shared.h"


int main() {
    int fd = shm_open(shared_memory_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); handle(fd);

    handle(ftruncate(fd, sizeof(shared_data)));

    shared_data *m = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); crash_on(m == MAP_FAILED);

    handle(sem_init(&m->read1, 1, 0));
    handle(sem_init(&m->read2, 1, 0));
    handle(sem_init(&m->write1, 1, 0));
    handle(sem_init(&m->write2, 1, 0));
    for (int i = 0; i < SIZE; ++i) m->buf[i] = '\0';
    m->active = 1;


    string filename1;
    printf("Input a filename in which you want to write output of child1: ");
    string_read(&filename1);

    string filename2;
    printf("Input a filename in which you want to write output of child2: ");
    string_read(&filename2);

    pid_t p_id = fork(); handle(p_id);

    if (p_id > 0) { // parent process
        p_id = fork(); handle(p_id);

        if (p_id > 0) { // TRUE parent process
            printf("Input strings [CTRL+D TO EXIT]:\n");
            while (scanf("%s", m->buf) != EOF) {
                sem_post(&m->read1);
                sem_post(&m->read2);

                sem_wait(&m->write1);
                sem_wait(&m->write2);
            }
        } else { // child2
            char *argv[] = { "processor.out", filename2.values, "2", NULL };
            handle(execv(argv[0], argv));
        }
    } else { // child1
        char *argv[] = { "processor.out", filename1.values, "1", NULL };
        handle(execv(argv[0], argv));
    }

    m->active = 0;
    sem_post(&m->read1);
    sem_post(&m->read2);
    sem_wait(&m->write1);
    sem_wait(&m->write2);

    sem_destroy(&m->read1);
    sem_destroy(&m->read2);
    sem_destroy(&m->write1);
    sem_destroy(&m->write2);

    munmap(m, sizeof(shared_data));

    handle(shm_unlink(shared_memory_file));

    return 0;
}
