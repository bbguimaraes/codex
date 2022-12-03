#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>

using u8 = std::uint8_t;

u8 to_mask(bool b) {
    return -b;
}

u8 to_mask_u8(u8 x) {
    return to_mask(x);
}

namespace n0 {

u8 set(u8 x) {
    constexpr u8 f0 = 0x10, f1 = 0x04;
    const u8 m = to_mask_u8(x & f0);
    return x ^ (f1 & (x ^ m));
}

}

namespace n1 {

u8 set(u8 x, u8 f0, u8 f1) {
    const u8 m = to_mask_u8(x & f0);
    return x ^ (f1 & (x ^ m));
}

}

template<std::unsigned_integral T>
int tzcnt(T n) {
    return std::numeric_limits<T>::digits
        - std::popcount(static_cast<T>(n | -n));
}

void print(u8 x) {
    for(int i = 0; i != 8; ++i, x <<= 1)
        putchar('0' + (x >> 7));
}

void test(u8 x) {
    print(x), putchar(' '), print(n0::set(x)), putchar('\n');
}

int main(void) {
    test(0x05);
    test(0x01);
    test(0x11);
    test(0x15);
}
