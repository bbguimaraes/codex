#include <array>
#include <bit>
#include <climits>
#include <concepts>
#include <cstdint>
#include <limits>
#include <numeric>
#include <span>

using u8 = std::uint8_t;
using u32 = std::uint32_t;

namespace linear {

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    constexpr int w = std::numeric_limits<T>::digits;
    int ret = 0;
    for(int i = 0; i != w; ++i, n >>= 1)
        ret += static_cast<int>(n & T{1});
    return ret;
}

}

namespace shift {

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    int ret = 0;
    for(; n; n >>= 1)
        ret += static_cast<int>(n & T{1});
    return ret;
}

}

namespace blsr {

template<std::unsigned_integral T>
constexpr T blsr(T n) {
    return n & (n - T{1});
}

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    int ret = 0;
    for(; n; n = blsr(n))
        ++ret;
    return ret;
}

}

namespace lib {

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    return std::popcount(n);
}

}

namespace table {

static constexpr auto t = [] {
    std::array<int, 1 << CHAR_BIT> t;
    for(unsigned i = 0; i != t.size(); ++i)
        t[i] = std::popcount(i);
    return t;
}();

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    constexpr int b = CHAR_BIT;
    constexpr int s = sizeof(T);
    constexpr int m =
        static_cast<unsigned char>(-1);
    int ret = 0;
    for(int i = 0; i != s; ++i, n >>= b)
        ret += t[n & m];
    return ret;
}

}

namespace sum {

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    for(T x = n; (x >>= 1);)
        n -= x;
    return n;
}

}

namespace rot {

template<std::unsigned_integral T>
constexpr int popcnt(T n) {
    constexpr int w = std::numeric_limits<T>::digits;
    T ret = 0;
    for(int i = 0; i != w; ++i, n = std::rotl(n, 1))
        ret += n;
    return -static_cast<std::make_signed_t<T>>(ret);
}

}

namespace bin {

template<std::unsigned_integral T, unsigned w>
constexpr T mask(void) {
    constexpr int n = std::numeric_limits<T>::digits;
    static_assert(std::has_single_bit(w));
    static_assert(w < n);
    T ret = (T{1} << w) - 1;
    for(unsigned i = n; !((i >>= 1) & w);)
        ret |= ret << i;
    return ret;
}

template<std::unsigned_integral T, unsigned w = 1>
constexpr int popcnt(T n) {
    if constexpr(w == std::numeric_limits<T>::digits)
        return n;
    else {
        constexpr auto m = mask<T, w>();
        n = static_cast<T>((n & m) + ((n >> w) & m));
        return popcnt<T, w * 2>(n);
    }
}

}

namespace csa {

template<std::unsigned_integral T>
constexpr T csa(T *t0, T t1, T t2) {
    const T x = *t0 ^ t1;
    const T ret = (*t0 & t1) | (x & t2);
    *t0 = x ^ t2;
    return ret;
}

template<std::unsigned_integral T>
constexpr int popcnt(std::span<const T> s) {
    int high = 0;
    T low = 0;
    for(std::ptrdiff_t i = 0, n = std::ssize(s) - 2; i <= n; i += 2)
        high += std::popcount(csa(&low, s[i], s[i + 1]));
    int ret = 2 * high + std::popcount(low);
    if(s.size() & 1)
        ret += std::popcount(s.back());
    return ret;
}

}

using U = u8;
//using U = u32;
template int linear::popcnt<U>(U);
template int shift::popcnt<U>(U);
template int blsr::popcnt<U>(U);
template int lib::popcnt<U>(U);
template int table::popcnt<U>(U);
template int bin::popcnt<U, 1>(U);
template int sum::popcnt<U>(U);
template int rot::popcnt<U>(U);
template int csa::popcnt<U>(std::span<const U>);

template<typename U, int f(U)>
bool test(void) {
    U n = 0;
    do {
        if(f(n) != std::popcount(n))
            return false;
    } while(++n);
    return true;
}

template<typename U, int f(std::span<const U>)>
bool test_array(void) {
    constexpr auto d = std::numeric_limits<U>::digits;
    if constexpr(CHAR_BIT < d)
        return true;
    else {
        constexpr auto a = [] {
            std::array<U, std::size_t{1} << d> ret;
            std::iota(begin(ret), end(ret), U{});
            return ret;
        }();
        constexpr auto c = a.size() * d / 2;
        static_assert(
            c == std::accumulate(
                begin(a), end(a), int{},
                [](int x, U y) { return x + std::popcount(y); }));
        return f(a) == c;
    }
}

int main(void) {
    return !(
        test<U, linear::popcnt<U>>()
        && test<U, shift::popcnt<U>>()
        && test<U, blsr::popcnt<U>>()
        && test<U, lib::popcnt<U>>()
        && test<U, table::popcnt<U>>()
        && test<U, bin::popcnt<U>>()
        && test<U, sum::popcnt<U>>()
        && test<U, rot::popcnt<U>>()
        && test_array<U, csa::popcnt<U>>()
    );
}
