#include <stdio.h>
#include <stdlib.h>

#include "prime.h"
#include "rebase.h"


int main() {
    int run = 1;
    int impl = 0;


    while (run) {
        int command = 0;
        printf(
            "Usage:\n"
            "\t\b-1 - exit\n"
            "\t0 - change implementation\n"
            "\t1 - count primes\n"
            "\t2 - rebase a number\n"
            "Command: "
        );
        scanf("%d", &command);

        if (command == -1) {
            run = 0;
        } else if (command == 0) {
            // change implementation
            impl = 1 - impl;
        } else if (command == 1) {
            // prime
            int a = 0, b = 0;
            printf(
                "Input A and B to find out how many primes are in [A, B]:\n"
                "\tA: "
            );
            scanf("%d", &a);
            printf("\tB: ");
            scanf("%d", &b);

            int result = 0;

            if (impl == 0) {
                result = prime_count_eratosthenes(a, b);
            } else {
                result = prime_count_stupid(a, b);
            }

            printf("There are %d primes in [%d, %d]!\n", result, a, b);
        } else if (command == 2) {
            // rebase
            int n = 0;
            printf("Input a number you want to rebase to base %d: ", impl + 2);
            scanf("%d", &n);

            char* result = NULL;

            if (impl == 0) {
                result = to_base2(n);
            } else {
                result = to_base3(n);
            }

            printf("The %d in base %d is %s!\n", n, impl + 2, result);
            free(result);
        }
    }


    return 0;
}