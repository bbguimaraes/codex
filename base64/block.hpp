#ifndef CODEX_BASE64_BLOCK_H
#define CODEX_BASE64_BLOCK_H

#include <cassert>
#include <cerrno>
#include <concepts>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>

/**
 * Processes an input buffer.
 *
 * Its argument indicates the size of the input buffer.  The return value is the
 * size of the output buffer.
 */
template<typename T> concept ProcessingFunc =
    std::same_as<
        std::invoke_result_t<T, std::size_t>,
        std::size_t>;

inline std::optional<std::size_t> read_all(
    FILE *f, std::size_t size, std::span<char> s);
inline bool write_all(FILE *f, std::string_view s);
inline bool process_blocks(
    FILE *in, FILE *out, std::size_t size,
    std::span<char> in_s, std::span<char> out_s,
    ProcessingFunc auto f);

/**
 * Reads a block of a specific size.
 *
 * Fills `s` with data read from `f`.  Reads until the buffer is filled or
 * `EOF`, in which case the buffer will be filled with zeroes until it is a
 * multiple of `size` in length.  `s` must also be a multiple of `size` in
 * length.  Returns the number of bytes read without the added zeroes, or
 * `nullopt` in case of errors..
 */
std::optional<std::size_t> read_all(
    FILE *f, std::size_t size, std::span<char> s
) {
    assert(s.size() % size == 0);
    std::size_t n = 0;
    do {
        n += fread(s.data() + n, 1, s.size() - n, f);
        if(ferror(f)) {
            std::cerr << "fread: " << std::strerror(errno) << '\n';
            return {};
        }
    } while(!feof(f) && n != s.size());
    if(const auto r = n % size)
        std::memset(s.data() + n, 0, size - r);
    return {n};
}

/**
 * Writes the entire content of `s` into an output stream.
 */
bool write_all(FILE *f, std::string_view s) {
    for(std::size_t n = 0; n != s.size();) {
        n += fwrite(s.data() + n, 1, s.size() - n, f);
        if(ferror(f)) {
            std::cerr << "fwrite: " << std::strerror(errno) << '\n';
            return false;
        }
    }
    return true;
}

/**
 * Reads block of a specific size, processes them, and outputs.
 *
 * Blocks of `size` are read from `in`, processed through `f`, and written to
 * `out`.  `in_s` and `out_s` are used as storage.
 */
bool process_blocks(
    FILE *in, FILE *out, std::size_t size,
    std::span<char> in_s, std::span<char> out_s,
    ProcessingFunc auto f
) {
    while(!feof(in)) {
        if(const auto n = read_all(in, size, in_s); !n)
            return false;
        else if(const auto ret = f(*n); !ret)
            return false;
        else if(!write_all(out, {out_s.data(), ret}))
            return false;
    }
    return true;
}

#endif
