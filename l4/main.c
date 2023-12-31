#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#define crash_on(boolean) if (boolean) { fprintf(stderr, "Error at file %s, line %d\n", __FILE__, __LINE__); exit(1); }
#define handle(i) crash_on(i == -1)

static int impl = 0;
static void *library = NULL;

static int (*prime_count)(int, int);
static char* (*to_base)(long);


void load_lib() {
    if (impl == 0) {
        library = dlopen("./libimpl1.so", RTLD_LAZY);
    } else {
        library = dlopen("./libimpl2.so", RTLD_LAZY);
    }
    crash_on(library == NULL);

    prime_count = (int(*)(int, int))dlsym(library, "prime_count");
    to_base = (char*(*)(long))dlsym(library, "to_base");
}


int main() {
    int run = 1;

    load_lib();

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
            dlclose(library);
            load_lib();
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
            printf("Input a number you want to rebase to base %d: ", impl + 2);
            scanf("%d", &n);

            char* result = NULL;

            result = to_base(n);

            printf("The %d in base %d is %s!\n", n, impl + 2, result);
            free(result);
        }
    }


    return 0;
}