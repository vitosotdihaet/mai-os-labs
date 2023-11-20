#include "shared.h"

#include "string.h"


string remove_vowels(string s) {
    string new_s;
    string_init(&new_s);

    uint64_t l = string_len(s);

    for (uint64_t i = 0; i < l; ++i) {
        char curr = (char) string_at(s, i);
        if (!(curr == 'a' || curr == 'e' || curr == 'i' || curr == 'o' || curr == 'u')) {
            string_push_char(&new_s, curr);
        }
    }

    return new_s;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Not enough arguments!");
        exit(1);
    }


    FILE *out = fopen(argv[1], "w+");

    if (out == NULL) {
        printf("Could not open a file with name %s\n", argv[1]);
        exit(2);
    }

    int child_id = atoi(argv[2]);


    int fd = shm_open(shared_memory_file, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); handle(fd);

    shared_data *m = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); crash_on(m == MAP_FAILED);


    sem_wait(&m->read1);
    printf("The data inside m->buf is: %s\n", m->buf.values);

    while (m->active) {
        printf("Hello from processor loop %d\n", child_id);

        if (child_id == 1) {
            sem_wait(&m->read1);
            printf("[?] CHILD ONE WAITED ENOUGH!\n");
            // printf("READ STRING IN CHILD 1 IS %s\n", m->buf.values);


            int l = string_len(m->buf);
            printf("The length is %d\n", l);
            if (l < 10) {
                printf("I'm in...\n");
                string processed = remove_vowels(m->buf);
                printf("String after removing vowels: %s\n", processed.values);
                fprintf(out, "%s\n", processed.values);
                printf("Why doesn't it work...\n");
            }

            sem_post(&m->write1);
            printf("CHILD ONE IS DONE\n");
        } else if (child_id == 2) {
            sem_wait(&m->read2);
            printf("WOWWWW child 2!\n");
            // printf("READ STRING IN CHILD 2 IS %s\n", m->buf.values);


            int l = string_len(m->buf);
            if (l >= 10) {
                string processed = remove_vowels(m->buf);
                printf("String after removing vowels: %s\n", processed.values);
                fprintf(out, "%s\n", processed.values);
            }
            sem_post(&m->write2);
            printf("CHILD TWO IS DONE\n");
        } else {
            sem_wait(&m->read2);
            printf("[?] CHILD TWO WAITED ENOUGH!\n");
            sem_post(&m->write2);
        }

    }
    printf("WE ARE OUT OF THE LOOP %d!\n", child_id);

    if (child_id == 1) sem_post(&m->write1);
    else sem_post(&m->write2);

    fclose(out);

    return 0;
}