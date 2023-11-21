#include <stdio.h>
#include <stdlib.h>

#include <string.h>


char* to_base2(long n) {
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

char* to_base3(long n) {
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