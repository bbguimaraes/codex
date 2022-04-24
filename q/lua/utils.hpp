#ifndef CODEX_LUA_UTILS_H
#define CODEX_LUA_UTILS_H

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string_view>
#include <utility>

#define FWD(x) std::forward<decltype(x)>(x)

template<std::size_t N>
struct fixed_string {
    constexpr fixed_string(void) = default;
    constexpr fixed_string(const char (&s_)[N + 1]);
    constexpr operator std::string_view(void) const { return {this->s, N}; }
    constexpr std::size_t size(void) const { return N; }
    constexpr const char *data(void) const { return this->s; }
    constexpr const char *begin(void) const { return this->s; }
    constexpr const char *end(void) const { return this->s + N; }
    constexpr char *begin(void) { return this->s; }
    constexpr char *end(void) { return this->s + N; }
    char s[N] = {};
};

template<typename ...Ts> struct types;

auto front(std::tuple<>) -> types<>;
auto pop_front(std::tuple<>) -> types<>;

template<typename T, typename ...Ts>
auto front(std::tuple<T, Ts...>) -> T;

template<typename T, typename ...Ts>
auto pop_front(std::tuple<T, Ts...>) -> types<Ts...>;

template<typename ...Ts>
struct types {
    using front = decltype(::front(std::tuple<Ts...>{}));
    template<typename T> using push_back = types<Ts..., T>;
    using pop_front = decltype(::pop_front(std::tuple<Ts...>{}));
    static constexpr void for_each(auto &&f) { (..., FWD(f)(Ts{})); }
};

template<std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

template<std::size_t N>
constexpr fixed_string<N>::fixed_string(const char (&s_)[N + 1]) {
    std::copy(s_, s_ + N, this->s);
}

template<std::size_t N0, std::size_t N1>
constexpr auto operator+(fixed_string<N0> s0, fixed_string<N1> s1) {
    auto ret = fixed_string<N0 + N1>{};
    std::copy(s0.begin(), s0.end(), ret.begin());
    std::copy(s1.begin(), s1.end(), ret.begin() + N0);
    return ret;
}

template<fixed_string s, std::size_t b, std::size_t e>
struct fixed_string_view {
    constexpr explicit operator std::string_view(void) const;
};

template<fixed_string s, std::size_t b, std::size_t e>
constexpr fixed_string_view<s, b, e>::operator std::string_view(void) const {
    return std::string_view{s}.substr(b, e - b);
}

template<auto x>
using constant = std::integral_constant<decltype(x), x>;

constexpr auto slice(std::string_view s, std::size_t b, std::size_t e) {
    return std::string_view{
        begin(s) + std::min(b, s.size()),
        begin(s) + std::min(e, s.size()),
    };
}

#endif
