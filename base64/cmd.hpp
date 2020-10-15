#ifndef CODEX_BASE64_CMD_H
#define CODEX_BASE64_CMD_H

#include <concepts>
#include <cstddef>
#include <functional>
#include <vector>

#include "block.hpp"

/**
 * Number of 3- or 4-byte blocks in the input buffer.
 */
constexpr std::size_t BUF_SIZE = 32 * 1024;

template<typename T> concept EncodeFunc =
    std::invocable<T, std::string_view, char*>;

template<typename T> concept DecodeFunc =
    std::same_as<
        std::invoke_result_t<T, std::string_view, char*>,
        char*>;

/**
 * Rounds an integral value to a given block size, always towards infinity.
 */
template<auto S, std::integral T>
inline constexpr T round_to_block_size(T n);

/**
 * Calculates the size of the buffer required for the output, rounding up.
 * \tparam N output block size.
 * \tparam D input block size.
 * \param n: size of the input.
 */
template<std::size_t N, std::size_t D>
inline constexpr std::size_t output_size(std::size_t n);

inline bool cmd_encode(EncodeFunc auto f);
inline bool cmd_decode(DecodeFunc auto f);
inline bool cmd_main(const char **argv, EncodeFunc auto ef, DecodeFunc auto df);

template<auto S, std::integral T>
constexpr T round_to_block_size(T n)
    { return S * ((n + S - 1) / S); }

template<std::size_t N, std::size_t D>
constexpr std::size_t output_size(std::size_t n)
    { return N * ((n + D - 1) / D); }

bool cmd_encode(EncodeFunc auto f) {
    std::vector<char> in(3 * BUF_SIZE), out(output_size<4, 3>(in.size()));
    return process_blocks(
        stdin, stdout, 3, in, out,
        [&in, &out, &f](std::size_t n) {
            std::invoke(f, std::string_view{in.data(), n}, out.data());
            return output_size<4, 3>(n);
        });
}

bool cmd_decode(DecodeFunc auto f) {
    std::vector<char> in(4 * BUF_SIZE), out(output_size<3, 4>(in.size()));
    return process_blocks(
        stdin, stdout, 4, in, out,
        [&in, &out, &f](std::size_t n) {
            const auto *ret =
                std::invoke(f, std::string_view{in.data(), n}, out.data());
            [[unlikely]] if(!ret)
                return (std::cerr << "invalid input\n"), std::size_t{};
            return static_cast<std::size_t>(ret - out.data());
        });
}

template<typename T>
bool cmd_main(const char **argv) {
    const bool encode = !argv[1];
    if(!encode && strcmp(argv[1], "-d") != 0) {
        std::cerr << "invalid command: " << argv[1] << '\n';
        return 1;
    }
    return encode ? cmd_encode(&T::encode) : cmd_decode(&T::decode);
}

#define BASE64_MAIN(T) \
    int main(int, const char **argv) { return !cmd_main<T>(argv); }

#endif
