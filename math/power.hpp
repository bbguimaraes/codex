#ifndef CODEX_MATH_POWER_H
#define CODEX_MATH_POWER_H

#include <cassert>
#include <cmath>
#include <concepts>

namespace detail {

template<typename T>
constexpr T power(T b, T e) {
    return e == T{} ? T{1} : b * power(b, e - T{1});
}

}

template<typename T>
concept arithmetic = requires (T t0, T t1) {
    t0 +  t1; t0 -  t1; t0 *  t1; t0 /  t1; t0 %  t1;
    t0 += t1; t0 -= t1; t0 *= t1; t0 /= t1; t0 %= t1;
};

template<typename T>
requires
    std::semiregular<T>
    && std::constructible_from<T, int>
    && std::three_way_comparable<T>
    && arithmetic<T>
T power(T b, T e) {
    assert(e >= T{0});
    constexpr auto pow = detail::power<T>;
    [[maybe_unused]] const T ib = b, ie = e;
    T ret = T{1}; // x ^ y == 1 * x ^ y
    for(; e != T{}; e /= T{2}) {
        assert(ret * pow(b, e) == pow(ib, ie));
        if(e % T{2} == T{1}) {
            // x ^ (y + 1) == x * x ^ y
            assert(pow(b, e) == b * pow(b, e - T{1}));
            ret *= b;
        }
        // x ^ y == (x * x) ^ (y / 2)
        assert(pow(b, e - (e % T{2})) == pow(b * b, e / T{2}));
        b *= b;
    }
    assert(ret * pow(b, e) == pow(ib, ie)); // r * x ^ 0 == r
    assert(ret == pow(ib, ie));
    return ret;
}

#endif
