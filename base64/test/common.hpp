#ifndef CODEX_BASE64_TEST_COMMON_H
#define CODEX_BASE64_TEST_COMMON_H

#include <functional>
#include <random>
#include <string_view>
#include <vector>
#include <tuple>

#include "cmd.hpp"

template<typename T>
std::vector<char> rnd_vec(T *gen, std::size_t n) {
    std::vector<char> ret(round_to_block_size<3>(n));
    std::generate(
        begin(ret), begin(ret) + static_cast<std::ptrdiff_t>(n),
        std::bind_front(std::uniform_int_distribution<char>{}, *gen));
    return ret;
}

template<typename T>
auto gen_input(T *gen) {
    QFETCH(std::size_t, dec_size);
    dec_size = round_to_block_size<3>(dec_size);
    const auto enc_size = output_size<4, 3>(dec_size);
    return std::tuple(dec_size, enc_size, rnd_vec(gen, dec_size));
}

template<typename T, typename C, bool B>
void encode_test(auto *gen) {
    const auto [dec_size, enc_size, input_v] = gen_input(gen);
    const std::string_view input = {input_v.data(), dec_size};
    auto output = std::vector<char>(enc_size);
    if constexpr(B)
        QBENCHMARK { T::encode(input, output.data()); }
    else
        T::encode(input, output.data());
    check_encode<T::encode>(input, output);
}

template<typename T, typename C, bool B>
void decode_test(auto *gen) {
    const auto [dec_size, enc_size, input_v] = gen_input(gen);
    const std::string_view input = {input_v.data(), dec_size};
    auto encoded = std::vector<char>(enc_size);
    T::encode(input, encoded.data());
    auto output = std::vector<char>(input_v.size());
    if constexpr(B)
        QBENCHMARK { T::decode({encoded.data(), enc_size}, output.data()); }
    else
        T::decode({encoded.data(), enc_size}, output.data());
    check_decode<T::decode>(input, {encoded.data(), encoded.size()}, output);
}

template<auto F>
void check_encode(std::string_view input, const std::vector<char> &output) {
    const auto n = output.size();
    auto expected = std::vector<char>(n);
    F(input, expected.data());
    QCOMPARE(
        std::string_view(output.data(), n),
        std::string_view(expected.data(), n));
}

template<auto F>
void check_decode(
    std::string_view input, std::string_view encoded,
    const std::vector<char> &output
) {
    const auto n = input.size();
    auto expected = std::vector<char>(n);
    QCOMPARE(
        static_cast<const void*>(expected.data() + n),
        static_cast<const void*>(F(encoded, expected.data())));
    QCOMPARE(
        std::string_view(output.data(), n),
        std::string_view(expected.data(), n));
}

#endif
