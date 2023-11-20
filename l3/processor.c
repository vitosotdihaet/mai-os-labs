#include "shared.h"


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
    if (argc < 2) {
        printf("Not enough arguments!");
        exit(1);
    }

    FILE *out = fopen(argv[1], "w+");

    if (out == NULL) {
        printf("Could not open a file with name %s\n", argv[1]);
        exit(2);
    }

    string s;
    while (!string_read(&s)) {
        string processed = remove_vowels(s);

        printf("String after removing vowels: %s\n", processed.values);

        fprintf(out, "%s\n", processed.values);
    }

    fclose(out);

    return 0;
}