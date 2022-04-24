#ifndef CODEX_LUA_LUA_H
#define CODEX_LUA_LUA_H

#include <array>
#include <cstddef>

static constexpr std::size_t MAX = 32;

enum class node {
    identifier, number, string,
    table_begin, table_key, table_value, table_end, n,
};
constexpr std::array node_names = {
    "identifier", "number", "string",
    "table_begin", "table_key", "table_value", "table_end",
};
static_assert(static_cast<std::size_t>(node::n) == node_names.size());

#endif
