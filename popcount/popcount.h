#ifndef CODEX_POPCOUNT_POPCOUNT_H
#define CODEX_POPCOUNT_POPCOUNT_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>

size_t popcount(size_t n) {
    return __builtin_popcountll(n);
}

size_t popcount0(size_t n) {
    if(!n)
        return 0;
    const size_t max = sizeof(size_t) * CHAR_BIT;
    size_t ret = 0;
    for(size_t i = 0; i < max; ++i)
        ret += !!(n & (((size_t)1) << i));
    return ret;
}

size_t popcount1(size_t n) {
    if(!n)
        return 0;
    size_t ret = 0;
    for(size_t i = 1; i; i <<= 1)
        ret += !!(n & i);
    return ret;
}

size_t popcount2(size_t n) {
    size_t ret = 0;
    for(; n; n >>= 1)
        ret += n & 1;
    return ret;
}

void test(size_t n) {
    const size_t p = popcount(n);
    const size_t p0 = popcount0(n);
    const size_t p1 = popcount1(n);
    const size_t p2 = popcount2(n);
    printf(
        "%zx %zu %zu %zu %zu %u\n",
        n, p, p0, p1, p2,
        p == p0 && p == p1 && p == p2);
}

int main() {
    const size_t v[] = {0, 1, 2, 3, 4, 5, 6, 6, (size_t)1 << 63};
    const size_t n = sizeof(v) / sizeof(*v);
    for(size_t i = 0; i < n; ++i)
        test(v[i]);
}
#endif
