#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define max(a, b) a > b ? a : b


int prime_count(int a, int b) {
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


char* to_base(long n) {
    int length = 0;
    long temp = n;

    while (temp > 0) {
        temp /= 3;
        length++;
    }


    char *result = calloc(length + 1, sizeof(char));
    
    for (int i = length - 1; i >= 0; i--) {
        result[i] = (n % 3) + '0';
        n /= 3;
    }

    return result;
}