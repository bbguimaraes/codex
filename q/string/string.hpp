#ifndef CODEX_STRING_STRING_H
#define CODEX_STRING_STRING_H

#include <cstring>
#include <string_view>

namespace codex {

inline std::string_view remove_prefix_tern(
    std::string_view s, std::string_view p
) {
    return s.substr(s.starts_with(p) ? p.size() : 0);
}

inline std::string_view remove_prefix_mul(
    std::string_view s, std::string_view p
) {
    return s.substr(s.starts_with(p) * p.size());
}

inline std::string_view remove_prefix_c(
    std::string_view s, std::string_view p
) {
    return s.substr(
        (!p.empty() && std::memcmp(s.data(), p.data(), p.size()) == 0)
        ? p.size() : 0);
}

inline std::string_view remove_prefix_c_strncmp(
    std::string_view s, std::string_view p
) {
    const char *const t = (std::strncmp(s.data(), p.data(), p.size()) == 0)
        ? s.data() + p.size() : s.data();
    return {t, &*end(s)};
}

}

#endif
