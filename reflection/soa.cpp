#include <iostream>
#include <ranges>

#include "soa.hpp"

struct E { float p, v, a; const char *n; };
using SOA = codex::refl::SOA<E>;

using T = codex::refl::field_tuple_t<E>;
static_assert(std::same_as<T, decltype(codex::refl::field_tuple(E{}))>);
static_assert(codex::refl::field_count<E>() == 4);
static_assert(std::tuple_size_v<T> == 4);
static_assert(std::same_as<std::tuple_element_t<0, T>, float&>);
static_assert(std::same_as<std::tuple_element_t<1, T>, float&>);
static_assert(std::same_as<std::tuple_element_t<2, T>, float&>);
static_assert(std::same_as<std::tuple_element_t<3, T>, const char*&>);
static_assert(std::is_same_v<SOA::field_type<0>, float>);
static_assert(std::is_same_v<SOA::field_type<1>, float>);
static_assert(std::is_same_v<SOA::field_type<2>, float>);
static_assert(std::is_same_v<SOA::field_type<3>, const char*>);
static_assert(std::ranges::contiguous_range<decltype(SOA{}.field<0>())>);
static_assert(std::ranges::contiguous_range<decltype(SOA{}.field<1>())>);
static_assert(std::ranges::contiguous_range<decltype(SOA{}.field<2>())>);
static_assert(std::ranges::contiguous_range<decltype(SOA{}.field<3>())>);

void f(E *e) {
    auto t = codex::refl::field_tuple(*e);
    std::apply([](const auto &...x) { (..., (std::cout << x << '\n')); }, t);
    std::get<1>(t) = 42.0f;
}

int main(void) {
    SOA v = {};
    v.push_back({.p = 0, .v = 1, .a = 2, .n = "e0"});
    v.push_back({.p = 3, .v = 4, .a = 5, .n = "e1"});
    v.push_back({.p = 6, .v = 7, .a = 8, .n = "e2"});
    v.push_back();
    v.set(3, {.p = 9, .v = 10, .a = 11, .n = "e3"});
    std::printf("size: %zu\n", v.size());
    const auto e0 = v[0];
    std::printf("e0: %g %g %g %s\n", e0.p, e0.v, e0.a, e0.n);
    const auto f0 = v.field<0>();
    const auto f1 = v.field<1>();
    const auto f2 = v.field<2>();
    const auto f3 = v.field<3>();
    std::printf("p:");
    for(const auto &x : f0)
        std::printf(" %g", x);
    std::printf("\nv:");
    for(const auto &x : f1)
        std::printf(" %g", x);
    std::printf("\na:");
    for(const auto &x : f2)
        std::printf(" %g", x);
    std::printf("\nn:");
    for(const auto &x : f3)
        std::printf(" %s", x);
}
