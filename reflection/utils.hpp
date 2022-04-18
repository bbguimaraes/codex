#ifndef CODEX_REFLECTION_UTILS_HPP
#define CODEX_REFLECTION_UTILS_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

#define CODEX_FWD(x) std::forward<decltype(x)>(x)

namespace codex::refl {

/** Alias for an `integral_constant` of `size_t`. */
template<std::size_t I>
using index_constant = std::integral_constant<std::size_t, I>;

}

#endif
