#ifndef CODEX_UTIL_UTIL_H
#define CODEX_UTIL_UTIL_H

namespace codex::literals {

inline constexpr std::size_t operator ""_zu(unsigned long long int i) {
    return {i};
}

}

#endif
