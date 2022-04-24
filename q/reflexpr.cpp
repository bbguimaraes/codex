// g++ -std=c++20 -masm=intel -fno-stack-protector -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -llua -O2 %
#include <string>
#include <tuple>

#define FWD(x) (std::forward<decltype(x)>(x))
#define reflexpr(T) std::experimental::reflect::detail::reflexpr_impl<T>

template<typename T> struct member_pointer_type;

template<typename T, typename P>
struct member_pointer_type<P T::*> { using type = P; };

namespace std::experimental::reflect {

namespace detail {

struct object_tag;
struct object_seq_tag;

}

template<typename T>
concept Object = std::derived_from<T, detail::object_tag>;

template<typename T>
concept ObjectSequence = std::derived_from<T, detail::object_seq_tag>;

namespace detail {

struct object_tag {};
struct object_seq_tag {};

template<typename T> struct reflexpr_impl { using type = T; };

template<Object ...Os>
struct ObjectSeqImpl : object_seq_tag { using types = std::tuple<Os...>; };

template<auto p>
struct data_member : object_tag {
    using type = typename member_pointer_type<decltype(p)>::type;
    static constexpr auto value = p;
};

}

namespace test {

template<Object ...Os>
using make_sequence = detail::ObjectSeqImpl<Os...>;

template<auto p>
using make_member_data = detail::data_member<p>;

template<typename T>
struct get_public_data_members {
    using type = make_sequence<
        make_member_data<&T::type::i>,
        make_member_data<&T::type::f>,
        make_member_data<&T::type::s>,
        make_member_data<&T::type::p>
    >;
};

template<typename T>
using get_public_data_members_t = get_public_data_members<T>::type;

}

template<Object O> using get_type = O;
template<Object O> using get_type_t = get_type<O>::type;

template<Object O> struct get_pointer : O {};
template<Object O> inline constexpr auto get_pointer_v = get_pointer<O>::value;

template<ObjectSequence S> using get_size = std::tuple_size<typename S::types>;
template<ObjectSequence S>
inline constexpr auto get_size_v = get_size<S>::value;

template<std::size_t I, ObjectSequence S>
using get_element = std::tuple_element<I, typename S::types>;
template<std::size_t I, ObjectSequence S>
using get_element_t = get_element<I, S>::type;

using test::get_public_data_members;
using test::get_public_data_members_t;

}

#include <concepts>
#include <iostream>
#include <string>
#include <tuple>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

template<typename T> concept pointer = std::is_pointer_v<T>;

template<typename T>
void for_each_public(T &&x, auto &&f) {
    namespace refl = std::experimental::reflect;
    using D = refl::get_public_data_members_t<reflexpr(std::decay_t<T>)>;
    [&x, &f]<std::size_t ...Is>(std::index_sequence<Is...>) {
        (..., FWD(f)(FWD(x).*refl::get_pointer_v<refl::get_element_t<Is, D>>));
    }(std::make_index_sequence<refl::get_size_v<D>>());
}

void lua_push(lua_State *L, int i) { lua_pushinteger(L, i); }
void lua_push(lua_State *L, float f) { lua_pushnumber(L, f); }
void lua_push(lua_State *L, pointer auto p) { lua_pushlightuserdata(L, p); }

void lua_push(lua_State *L, const std::string &s) {
    lua_pushlstring(L, s.data(), s.size());
}

template<typename T>
requires(!pointer<T>)
void lua_push(lua_State *L, const T &x) {
    for_each_public(x, [L](auto &&f) { lua_push(L, FWD(f)); });
}

template<typename T> struct lua_get_impl;

template<typename T>
decltype(auto) lua_get(lua_State *L, int i) {
    return lua_get_impl<T>::get(L, i);
}

template<>
struct lua_get_impl<int> {
    static int get(lua_State *L, int i) { return lua_tointeger(L, i); }
};

template<>
struct lua_get_impl<float> {
    static float get(lua_State *L, int i) { return lua_tonumber(L, i); }
};

template<pointer P>
struct lua_get_impl<P> {
    static P get(lua_State *L, int i) { return lua_touserdata(L, i); }
};

template<>
struct lua_get_impl<std::string> {
    static std::string get(lua_State *L, int i) {
        std::size_t n = 0;
        const char *const p = lua_tolstring(L, i, &n);
        return {p, n};
    }
};

template<typename T>
requires(!pointer<T>)
struct lua_get_impl<T> {
    static T get(lua_State *L, int i) {
        T ret = {};
        for_each_public(ret, [L, i = i]<typename F>(F &&f) mutable {
            f = lua_get<std::decay_t<F>>(L, i++);
        });
        return ret;
    }
};

struct S { double d; };

template<>
struct std::experimental::reflect::test::get_public_data_members<reflexpr(S)> {
    using type = make_sequence<make_member_data<&S::d>>;
};

int main() {
    struct {
        int i;
        float f;
        std::string s;
        void *p;
    } s = {
        .i = 42,
        .f = 43,
        .s = "test",
        .p = &std::cout,
    };
    lua_State *const L = luaL_newstate();
    lua_push(L, s);
    s = {};
    lua_pushinteger(L, 44);
    lua_replace(L, 1);
    s = lua_get<decltype(s)>(L, 1);
    *static_cast<std::ostream*>(s.p) << s.i << ' ' << s.f << ' ' << s.s << '\n';
    namespace refl = std::experimental::reflect;
    static_assert(
        std::same_as<
            double,
            refl::get_type_t<
                refl::get_element_t<0,
                    refl::get_public_data_members_t<reflexpr(S)>>>>);
}
