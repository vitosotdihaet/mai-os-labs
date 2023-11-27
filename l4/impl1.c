#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define max(a, b) a > b ? a : b


int prime_count(int a, int b) {
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

char* to_base(long n) {
    int length = 0;
    long temp = n;

    while (temp > 0) {
        temp /= 2;
        length++;
    }

    char *result = calloc(length + 1, sizeof(char));

    for (int i = length - 1; i >= 0; i--) {
        result[i] = (n % 2) + '0';
        n /= 2;
    }

    return result;
}