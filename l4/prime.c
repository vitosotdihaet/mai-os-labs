#include <stdlib.h>
#include <stdio.h>


#define max(a, b) a > b ? a : b


int prime_count_eratosthenes(int a, int b) {
    int count = 0;
    int *primes = calloc(b + 1, sizeof(int));

    if (primes == NULL) {
        fprintf(stderr, "Could not allocate memory for prime_count!\n");
        return 0;
    }

    for (int i = 0; i <= b; ++i) {
        primes[i] = 1;
    }

    for (int i = 2; i <= b; ++i) {
        if (primes[i] != 0) {
            for (int j = i * i; j <= b; j += i) primes[j] = 0;
        }
    }

    for (int i = max(2, a); i <= b; ++i) {
        if (primes[i] != 0) count++;
    }

    return count;
}

int prime_count_stupid(int a, int b) {
    int count = 0;

    for (int i = max(2, a); i <= b; ++i) {
        int bad = 0;

        for (int j = 2; j * j <= i; ++j) {
            if (i % j == 0) {
                bad = 1;
                break;
            }
        }

        if (!bad) count++;
    }

    return count;
}
