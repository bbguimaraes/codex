#ifndef CODEX_REFLECTION_FIELDS_HPP
#define CODEX_REFLECTION_FIELDS_HPP

#include "detail/fields.hpp"

namespace codex::refl {

/**
 * Number of fields in the `struct` \p T.
 * This is the specialization that is always instantiable.  It is the converse
 * of the `requires` clause in the other member of the overload set, i.e.
 * `struct` \p T cannot be constructed with \p N fields.  Because the search is
 * done linearly from zero, the structure must have `N - 1` fields.
 */
template<typename T, std::size_t ...N>
constexpr std::size_t field_count(void) {
    return sizeof...(N) - 1;
}

/**
 * Number of fields in the `struct` \p T.
 * Instantiable if it is possible to construct `struct` \p T with \p N fields.
 * \ref codex::refl::detail::to_any is used so that the types of the fields are
 * disregarded.  The `requires` clause will hold for values of \p N from zero to
 * the number of fields in \p T.
 */
template<typename T, std::size_t ...N>
requires(requires { T{detail::to_any<N>{}...}; })
constexpr std::size_t field_count(void) {
    return field_count<T, N..., sizeof...(N)>();
}

/** Constructs a tuple of references to each field of \p T, in order. */
template<typename T>
auto field_tuple(T &&t) {
    constexpr auto n = field_count<std::decay_t<T>>();
    return detail::field_tuple_impl<n>{}(std::forward<T>(t));
}

/** Alias of the tuple type returned by \ref field_tuple for type \p T. */
template<typename T>
using field_tuple_t = decltype(field_tuple(std::declval<T>()));

}

#endif
