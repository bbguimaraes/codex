#ifndef CODEX_PACKET_READER_PACKET_READER_HPP
#define CODEX_PACKET_READER_PACKET_READER_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>

namespace codex {

/** Data types that can be read from a buffer. */
enum class Type : uint8_t { INVALID, INT, FLOAT };

/** Mapping from \ref Type to C++ types. */
template<typename T> constexpr Type type_to_enum = Type::INVALID;
template<> constexpr Type type_to_enum<int> = Type::INT;
template<> constexpr Type type_to_enum<float> = Type::FLOAT;

/** Identifies whether a C++ type can be read from a buffer. */
template<typename T> concept Readable = type_to_enum<T> != Type::INVALID;

/** Arbitrary return type which could be replaced with a more detailed type. */
using ReadResult = bool;

/**
 * Wrapper for a raw byte buffer with methods to extract data from it.
 * Values in the buffer are in the form:
 * \code
 *     buffer: | Type    | value | Type    | value | ... |
 *     size:     uint8_t   size    uint8_t   size
 * \endcode
 * where \c size is \c sizeof(T) and \c T is such that <tt>type_to_enum<T></tt>
 * is the value in each preceding \ref Type value in the buffer.
 */
class Packet {
public:
    /** Maximum buffer size. */
    static constexpr std::size_t N = 32;
    /** Provides direct access to the underlying buffer. */
    std::span<char, N> data() { return this->buf; }
    /** Extracts typed data from the underlying raw byte buffer. */
    auto get(Readable auto *...ps) { return (this->read(ps) && ...); }
private:
    /** Advances \ref i after reading a type \c T. */
    template<typename T> void advance();
    /** Reads a sngle value of type \c T from the buffer. */
    template<typename T> T read();
    /**
     * Checks that the buffer contains a value of type \c T and reads it.
     * The current position in the buffer must contain a \ref Type value \c t
     * such that <tt>t == type_to_enum<T>.
     */
    template<typename T> ReadResult read(T *p);
    /** Raw byte buffer. */
    std::array<char, N> buf = {};
    /** Current offset into \ref buf.  Incremented after each \ref get. */
    size_t i = {};
};

template<typename T>
void Packet::advance() {
    const auto ni = this->i + sizeof(T);
    assert(ni < this->buf.size());
    this->i = ni;
}

template<typename T>
T Packet::read() {
    assert(this->i + sizeof(T) <= this->buf.size());
    T ret = {};
    std::memcpy(&ret, this->buf.data() + this->i, sizeof(T));
    return ret;
}

template<typename T>
ReadResult Packet::read(T *p) {
    if(this->read<Type>() != type_to_enum<T>)
        return false;
    this->advance<Type>();
    *p = this->read<T>();
    this->advance<T>();
    return true;
}

}

#endif
