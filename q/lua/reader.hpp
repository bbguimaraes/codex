#ifndef CODEX_LUA_READER_H
#define CODEX_LUA_READER_H

#include <cstddef>
#include <iostream>

#include <lua.hpp>

#include "gen.hpp"

template<std::size_t N>
struct X {
    using T = const char(&)[N];
    constexpr X(T s_) : s{s_} {}
    constexpr T operator--(void) { return this->s; }
    T s;
};

void read_number(lua_State *L) {
    std::cout << "number: " << lua_tonumber(L, -1) << '\n';
    lua_pop(L, 1);
}

void read_string(lua_State *L) {
    std::cout << "string: " << lua_tostring(L, -1) << '\n';
    lua_pop(L, 1);
}

void read_table(lua_State *L) {
    luaL_tolstring(L, -1, nullptr);
    std::cout << lua_tostring(L, -1) << '\n';
    lua_pop(L, 1);
    if(!lua_type(L, -1) == LUA_TTABLE)
        std::cerr << __func__ << ": invalid stack state\n";
}

void read_table_end(lua_State *L) {
    lua_pop(L, 1);
}

void read_table_key(lua_State *L, std::string_view k) {
    std::cout << "table_key: " << k << '\n';
    lua_pushlstring(L, k.data(), k.size());
    lua_gettable(L, -2);
}

void read(lua_State*, types<>) {}

template<typename T, typename ...Ts>
void read(lua_State *L, types<T, Ts...>) {
//    std::cerr << __PRETTY_FUNCTION__ << '\n';
    constexpr types<Ts...> next = {};
    if constexpr(std::derived_from<T, number_base>) {
        read_number(L);
        return read(L, next);
    } else if constexpr(std::derived_from<T, string_base>) {
        read_string(L);
        return read(L, next);
    } else if constexpr(std::same_as<T, table_begin>) {
        read_table(L);
        return read(L, next);
    } else if constexpr(std::same_as<T, table_key>) {
        read_table_key(L, std::string_view{types<Ts...>::front::value});
        return read(L, next);
    } else if constexpr(std::same_as<T, table_end>) {
        read_table_end(L);
        return read(L, next);
    } else
        return read(L, next);
}

#endif
