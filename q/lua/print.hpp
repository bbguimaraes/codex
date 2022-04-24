#ifndef CODEX_LUA_PRINT_H
#define CODEX_LUA_PRINT_H

#include <iostream>

#include "gen.hpp"
#include "utils.hpp"

template<fixed_string_view s>
std::ostream &operator<<(std::ostream &o, identifier<s>) {
    return o << "identifier<" << std::string_view{s} << '>';
}

template<fixed_string_view s>
std::ostream &operator<<(std::ostream &o, number<s>) {
    return o << "number<" << std::string_view{s} << '>';
}

template<fixed_string_view s>
std::ostream &operator<<(std::ostream &o, string<s>) {
    return o << "string<" << std::string_view{s} << '>';
}

std::ostream &operator<<(std::ostream &o, table_begin) {
    return o << "table_begin";
}

std::ostream &operator<<(std::ostream &o, table_key) {
    return o << "table_key";
}

std::ostream &operator<<(std::ostream &o, table_value) {
    return o << "table_value";
}

std::ostream &operator<<(std::ostream &o, table_end) {
    return o << "table_end";
}

#endif
