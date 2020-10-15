#ifndef CODEX_BASE64_BASE64_H
#define CODEX_BASE64_BASE64_H

#include <algorithm>
#include <iterator>
#include <limits>
#include <string_view>

using namespace std::literals;

/**
 * Performs a reverse look-up.
 * \param lut Look-up table.
 * \param c Character whose position in `lut` is returned.
 * \return The position of `c` or `lut.size()` if not found.
 */
consteval char lut_idx(std::string_view lut, char c) {
    const auto p = lut.data();
    const auto n = static_cast<char>(lut.size());
    return static_cast<char>(std::distance(p, std::find(p, p + n, c)));
}

/**
 * Creates an array that contains the position of every byte in `lut`.
 * \return
 *     An array `a` such that:
 *     - `a[c] == 0` if `c` is not found in `a` or is `=`
 *     - `lut[a[c]] == c` otherwise
 */
consteval auto gen_rlut(std::string_view lut) {
    std::array<char, std::numeric_limits<unsigned char>::max()> ret = {};
    for(std::size_t i = 0; i < ret.size(); ++i)
        ret[i] = lut_idx(lut, static_cast<char>(i));
    ret['='] = 0;
    return ret;
}

/**
 * Encodes and decodes byte streams using base64.
 */
class Base64 {
    /**
     * Look-up table for encoding, maps input 6-bit values to output characters.
     */
    static constexpr auto lut =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"sv;
    /**
     * Reverse look-up table for encoding, maps every possible character value
     * to a byte or zero.
     */
    static constexpr auto rlut = gen_rlut(lut);
public:
    /**
     * Encodes a byte stream.
     * \param src
     *     The underlying string must be a multiple of 3 in length, extra bytes
     *     beyond `src.size()` must be zero.
     * \param dst Must be at least `output_size<4, 3>(src.size())` in length.
     */
    static void encode(std::string_view src, char *dst);
    /**
     * Decodes a byte stream.
     * \param src The underlying string must be a multiple of 4 in length.
     * \param dst Must be at least `output_size<3, 4>(src.size())` in length.
     */
    static char *decode(std::string_view src, char *dst);
};

#endif
