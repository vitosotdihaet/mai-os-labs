#include "shared.h"

#include "string.h"
#include <string.h>


string remove_vowels(const char *s) {
    string new_s;
    string_init(&new_s);

    uint64_t l = strlen(s);

    for (uint64_t i = 0; i < l; ++i) {
        char curr = s[i];
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


    int fd = shm_open(shared_memory_file, O_RDWR, S_IRUSR | S_IWUSR); handle(fd);

    shared_data *m = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); crash_on(m == MAP_FAILED);

    while (m->active) {
        if (child_id == 1) {
            sem_wait(&m->read1);
            if (!m->active) break;

            int l = strlen(m->buf);
            if (l < 10) {
                string processed = remove_vowels(m->buf);
                printf("String after removing vowels %d: %s\n", child_id, processed.values);
                fprintf(out, "%s\n", processed.values);
            }

            sem_post(&m->write1);
        } else if (child_id == 2) {
            sem_wait(&m->read2);
            if (!m->active) break;

            int l = strlen(m->buf);
            if (l >= 10) {
                string processed = remove_vowels(m->buf);
                printf("String after removing vowels %d: %s\n", child_id, processed.values);
                fprintf(out, "%s\n", processed.values);
            }
            sem_post(&m->write2);
        }

    }

    if (child_id == 1) sem_post(&m->write1);
    else sem_post(&m->write2);

    fclose(out);

    return 0;
}
