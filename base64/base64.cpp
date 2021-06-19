#include "base64.hpp"

#include <bit>
#include <iostream>

namespace {

/**
 * Applies a mask and shifts right.
 *
 * The mask is applied to the input byte and the result is shifted right by the
 * amount of zero bits on the right side of the mask.
 */
template<unsigned char M>
constexpr auto rmask(char c)
    { return (c & M) >> std::countr_zero(M); }

/**
 * Shifts left and applies a mask.
 *
 * The input is shifted left by the amount of zero bits on the right side of the
 * mask and the mask is applied.
 */
template<unsigned char M>
constexpr auto lmask(char c)
    { return (c << std::countr_zero(M)) & M; }

/**
 * Merges parts of two bytes according to a mask.
 *
 * Applies `lmask` to `c0` and `rmask` to `c1` using `LM` and its opposite and
 * combines the result.
 */
template<unsigned char LM>
constexpr auto merge(char c0, char c1) {
    constexpr auto IM = static_cast<unsigned char>(~LM);
    constexpr auto RM = IM << std::countl_zero(IM);
    return (lmask<LM>(c0) | rmask<RM>(c1)) & 0x3f;
}

/**
 * Checks that no value is equal to `max`, which must be a power of 2.
 */
template<auto max>
constexpr bool check(auto ...vs) {
    using U = std::make_unsigned_t<decltype(max)>;
    static_assert(std::popcount(static_cast<U>(max)) == 1);
    return ~(vs | ...) & max;
}

/**
 * Looks up the value of byte `c` in look-up table `t`.
 */
constexpr auto lu(const auto &t, auto c)
    { return t[static_cast<unsigned char>(c)]; }

}

void Base64::encode(std::string_view src, char *dst) {
    constexpr auto &t = Base64::lut;
    for(const auto *p = src.data(), *const e = p + src.size(); p < e;) {
        const auto b0 = *p++, b1 = *p++, b2 = *p++;
        *dst++ = lu(t, rmask<0b11111100>(b0));
        *dst++ = lu(t, merge<0b11110000>(b0, b1));
        *dst++ = lu(t, merge<0b11111100>(b1, b2));
        *dst++ = lu(t, rmask<0b00111111>(b2));
    }
    switch(src.size() % 3) {
    case 1: dst[-2] = '='; [[fallthrough]];
    case 2: dst[-1] = '=';
    }
}

char *Base64::decode(std::string_view src, char *dst) {
    constexpr auto &t = Base64::rlut;
    const auto *p = src.data();
    for(const auto *const e = p + src.size(); p < e;) {
        const auto
            b0 = lu(t, *p++), b1 = lu(t, *p++),
            b2 = lu(t, *p++), b3 = lu(t, *p++);
        if(!check<Base64::lut.size()>(b0, b1, b2, b3)) [[unlikely]]
            return nullptr;
        *dst++ = static_cast<char>((b0 << 2) | (b1 >> 4));
        *dst++ = static_cast<char>((b1 << 4) | (b2 >> 2));
        *dst++ = static_cast<char>((b2 << 6) | (b3 >> 0));
    }
    if(!src.empty())
        while(*--p == '=')
            --dst;
    return dst;
}
