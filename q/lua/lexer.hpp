#ifndef CODEX_LUA_LEXER_H
#define CODEX_LUA_LEXER_H

#include "utils.hpp"

constexpr fixed_string SPACE = " \t\n";

constexpr fixed_string DIGITS = "0123456789";

constexpr fixed_string IDENT0 =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "_";

constexpr fixed_string IDENT1 = IDENT0 + DIGITS;

constexpr std::string_view ignore_interm(std::string_view s) {
    s = s.substr(s.find_first_not_of(SPACE));
    for(;;) {
        if(slice(s, 0, 2) != "--")
            return s;
        auto i = std::ranges::find(s, '\n');
        if(i != end(s))
            ++i;
        s = {i, end(s)};
    }
}

constexpr std::string_view consume(std::string_view s, std::string_view c) {
    if(!s.starts_with(c))
        throw "invalid prefix";
    return s.substr(c.size());
}

template<fixed_string s>
constexpr bool set_contains(char c) {
    return std::ranges::find(s, c) != s.end();
}

constexpr std::string_view consume_set_common(std::string_view s, auto i) {
    if(i == begin(s))
        throw "pattern not matched";
    return {i, end(s)};
}

template<fixed_string p>
constexpr std::string_view consume_set(std::string_view s) {
    return consume_set_common(s, std::ranges::find_if_not(s, set_contains<p>));
}

template<fixed_string p>
constexpr std::string_view consume_set_comp(std::string_view s) {
    return consume_set_common(s, std::ranges::find_if(s, set_contains<p>));
}

#endif
