#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include "string.h"


string remove_vowels(string s) {
    string new_s;
    init_string(&new_s);

    uint64_t l = string_len(s);

    for (uint64_t i = 0; i < l; ++i) {
        char curr = s.values[i];
        if (!(curr == 'a' || curr == 'e' || curr == 'i' || curr == 'o' || curr == 'u')) {
            push_char(&new_s, curr);
        }
    }

    return new_s;
}


int main(int argc, char *argv[]) {
    printf("Hello from processor\n");
    printf("I have %d args:\n", argc);

    for (int i = 0; i < argc; ++i) {
        printf("\t%s\n", argv[i]);
    }

    FILE *out = fopen(argv[1], "w+");

    if (out == NULL) {
        printf("Could not open a file with name %s\n", argv[1]);
        exit(1);
    }

    fprintf(out, "test string\n");

    string s;
    while (!read_string(&s)) {
        string processed = remove_vowels(s);

        printf("[DBUG] string in processor: %s\n", s.values);
        printf("[DBUG] after processing: %s\n", processed.values);

        fprintf(out, "%s\n", processed.values);
    }

    fclose(out);

    return 0;
}