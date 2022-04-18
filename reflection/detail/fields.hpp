#ifndef CODEX_REFLECTION_FIELDS_DETAIL_HPP
#define CODEX_REFLECTION_FIELDS_DETAIL_HPP

#include <cstdio>
#include <tuple>
#include <utility>

namespace codex::refl::detail {

/**
 * Structure which can be ostensibly converted to any other.
 * Used in the implementation of \ref codex::refl::field_count, where it is
 * placed in an aggregate construction such as `T{to_any<…>{}...}`.  The
 * conversion operation, much like `std::declval`, is purely declarative and
 * never implemented.
 */
template<auto>
struct to_any {
    template<typename T> constexpr operator T(void);
};

/**
 * Implementation of creating a `std::tuple` from the fields of a `struct`.
 * Each explicit specialization, generated with the macro below, binds the
 * fields of a `struct` with \p N fields.
 */
template<std::size_t N> struct field_tuple_impl;

// Could potentially be implemented in a more concise manner with auxiliary
// infrastructure such as Boost.Preprocessor.
#define X(n, ...) \
    template<> \
    struct field_tuple_impl<n> { \
        decltype(auto) operator()(auto &&x) { \
            auto &&[__VA_ARGS__] = std::forward<decltype(x)>(x); \
            return std::tie(__VA_ARGS__); \
        } \
    };
X(1, _0)
X(2, _0, _1)
X(3, _0, _1, _2)
X(4, _0, _1, _2, _3)
#undef X

}

#endif
