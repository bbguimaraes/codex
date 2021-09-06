#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include <immintrin.h>

typedef uint32_t u32;

static inline u32 U32_BIT = sizeof(u32) * CHAR_BIT;

u32 popcount0(u32 n) {
    return (u32)__builtin_popcount(n);
}

u32 popcount1(u32 n) {
    return (u32)_mm_popcnt_u32(n);
}

u32 popcount2(u32 n) {
    __asm__("popcnt %0, %1\n" : "+r" (n) : "r" (n));
    return n;
}

u32 popcount3(u32 n);
__asm__(
    "popcount3: popcnt %eax, %edi\n"
    "ret\n");

u32 popcount4(u32 n) {
    u32 ret = 0, mask = 1;
    do
        ret += !!(n & mask);
    while(mask <<= 1);
    return ret;
}

u32 popcount5(u32 n) {
    u32 ret = 0;
    for(u32 c = U32_BIT; c--; n >>= 1)
        ret += n & 1;
    return ret;
}

u32 popcount6(u32 n) {
    u32 ret = 0;
    for(; n; n >>= 1)
        ret += n & 1;
    return ret;
}

u32 popcount7(u32 n) {
    u32 ret = 0;
    while(n)
        __asm__(
            "shr %1, 1\n"
            "adc %0, 0\n"
            : "+r" (ret), "+r" (n));
    return ret;
}

u32 popcount8(u32 n);
__asm__(
    "popcount8:\n"
    "    xor rax, rax\n"
    ".L0:\n"
    "    shr %edi, 1\n"
    "    jz .L1\n"
    "    adc %eax, 0\n"
    "    jmp .L0\n"
    ".L1:\n"
    "    adc %eax, 0\n"
    "    ret\n");

u32 popcount9(u32 n) {
    u32 ret = 0;
    for(; n; n &= n - 1)
        ++ret;
    return ret;
}

u32 popcount10(u32 n) {
    u32 ret = 0;
    for(; n; n = _blsr_u32(n))
        ++ret;
    return ret;
}
