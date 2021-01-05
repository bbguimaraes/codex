#ifndef CODEX_LAMBDA_MUTABLE_H
#define CODEX_LAMBDA_MUTABLE_H

#include <array>
#include <cstddef>
#include <cstring>
#include <numeric>
#include <ranges>
#include <type_traits>

namespace {

/** Concept equivalent of the standard type trait. */
template<typename T> concept arithmetic = std::is_arithmetic_v<T>;

/** Helper function to reduce the number of casts. */
inline auto as_bytes(const void *p)
    { return static_cast<const std::byte*>(p); }

/** Overload set that determines the size in bytes of its argument. */
template<arithmetic T> inline auto arg_size(const T&) { return sizeof(T); }

/** Overload set that determines the address of its argument's content. */
inline auto arg_ptr(const arithmetic auto &t) { return as_bytes(&t); }

/** Overload for ranges of values. */
inline auto arg_size(const std::ranges::sized_range auto &r)
    { return r.size() * sizeof(decltype(*begin(r))); }

/** Overload for ranges of values. */
inline auto arg_ptr(const std::ranges::range auto &r)
    { return as_bytes(&*begin(r)); }

/** Equivalent to the lambda expression in \ref codex::copy_mutable_lambda. */
struct function_obj {
    std::byte *p;
    const std::size_t *s;
    void operator()(const auto &x) {
        std::memcpy(this->p, arg_ptr(x), *this->s);
        this->p += *this->s++;
    }
};

}

namespace codex {

/**
 * <tt>memcpy</tt>es each \c ts value into \c v (using a lambda expression).
 */
void copy_mutable_lambda(const auto &...ts, std::vector<std::byte> *v) {
    const std::array sizes = {arg_size(ts)...};
    v->resize(std::reduce(begin(sizes), end(sizes)));
    auto copy = [p = v->data(), s = sizes.data()](const auto &x) mutable
        { std::memcpy(p, arg_ptr(x), *s); p += *s++; };
    (..., copy(ts));
}

/**
 * <tt>memcpy</tt>es each \c ts value into \c v (using a function object).
 */
void copy_function_object(const auto &...ts, std::vector<std::byte> *v) {
    const std::array sizes = {arg_size(ts)...};
    v->resize(std::reduce(begin(sizes), end(sizes)));
    function_obj copy = {v->data(), sizes.data()};
    (..., copy(ts));
}

}

#endif
