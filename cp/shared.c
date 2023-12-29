#include "shared.h"


uint64_t closest_pow2(uint64_t n) {
    n -= 1;
    uint64_t result = 1;
    for (; n != 0; n /= 2) result *= 2;
    return result;
}

uint64_t closest_n_pow2(uint64_t n) {
    if (!n) return 0;
    n -= 1;
    uint64_t result = 0;
    for (; n != 0; n /= 2) result += 1;
    return result;
}

uint64_t pow2(uint64_t n) {
    return 1 << n;
}
