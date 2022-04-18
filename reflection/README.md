reflection
==========

Basic reflection of structures using C++17 structured bindings.  Similar in
spirit to `magic_get`/[Boost.PFR](https://boost.org/libs/pfr) but a completely
independent (and simplified) implementation.

```cpp
namespace refl = codex::refl;
struct E { float p, v, a; const char *n; };
// n.b. no other declarations necessary

static_assert(refl::field_count<E>() == 4);

using T = refl::field_tuple_t<E>;
static_assert(std::tuple_size_v<T> == 4);
static_assert(std::same_as<std::tuple_element_t<0, T>, float&>);
static_assert(std::same_as<std::tuple_element_t<1, T>, float&>);
static_assert(std::same_as<std::tuple_element_t<2, T>, float&>);
static_assert(std::same_as<std::tuple_element_t<3, T>, const char*&>);

void f(E *e) {
    auto t = refl::field_tuple(*e);
    std::apply([](const auto &...x) { (..., (std::cout << x << '\n')); }, t);
    std::get<1>(t) = 42.0f;
}
```

SOA
---

Container with an interface similar to `std::vector` but where each field of the
structure is transparently stored in its own contiguous array (i.e. a
structure-of-arrays, instead of an array-of-structures).

```cpp
refl::SOA<E> v = {};
// Insert using "aggregate constructor".
v.push_back({.p = 0, .v = 1, .a = 2, .n = "e0"});
// Insert default, set values using indexed "aggregate constructor".
v.push_back();
v.set(1, {.p = 3, .v = 4, .a = 5, .n = "e1"});
// Interface similar to std::vector.
std::printf("size: %zu\n", v.size());
const auto e0 = v[0];
std::printf("e0: %g %g %g %s\n", e0.p, e0.v, e0.a, e0.n);
// Access to contiguous field storage.
const auto f1 = v.field<1>();
static_assert(std::ranges::contiguous_range<decltype(f1)>);
std::printf("\nv:");
for(const auto &x : f1)
    std::printf(" %g", x);
```

A second template parameter can be used to specify the underlying storage
implementation for each field to another contiguous container (the default is
`std::vector<T>`).  Its `for_field` member is instantiated with the index and
type of each field.

```cpp
struct descriptor {
    template<std::size_t, typename T>
    using for_field = boost::container::static_vector<T, 1024>;
};
using SOA = refl::SOA<E, descriptor>;
```
