#include <stdio.h>
#include <stdlib.h>

#include "prime.h"
#include "rebase.h"


int main() {
    int run = 1;

    while (run) {
        int command = 0;
        printf(
            "Usage:\n"
            "\t\b-1 - exit\n"
            "\t1 - count primes\n"
            "\t2 - rebase a number\n"
            "Command: "
        );
        scanf("%d", &command);

        if (command == -1) {
            run = 0;
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

            result = prime_count(a, b);

            printf("There are %d primes in [%d, %d]!\n", result, a, b);
        } else if (command == 2) {
            // rebase
            int n = 0;
            printf("Input a number you want to rebase: ");
            scanf("%d", &n);

            char* result = NULL;

            result = to_base(n);

            printf("The %d is %s!\n", n, result);
            free(result);
        }
    }


    return 0;
}