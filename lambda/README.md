lambda
======

mutable
-------

This program demonstrates the use of a mutable lambda expression to copy
multiple sources passed as variadic template arguments into a single output byte
array.  The context where it originally appeared was populating a byte buffer to
be sent to the GPU via OpenCL:

```cpp
Compute *c = /* … */;
struct {
    std::uint32_t u; float f; std::array<float, 4> v;
} s = {1, 2, {3, 4, 5, 6}};
const auto src = c->create_buffer(
    Compute::MemFlag::READ_ONLY, sizeof(s), nullptr);
c->write_struct(src, {}, s.u, s.f, s.v);
```

The implementation is very concise, taking advantage of several language
features such as automatic type deduction for template parameters, fold
expressions, STL algorithms, and the aforementioned mutable lambda expression:

```cpp
void copy(const auto &...ts, std::vector<std::byte> *v) {
    const std::array sizes = {arg_size(ts)...};
    v->resize(std::reduce(begin(sizes), end(sizes)));
    auto copy = [p = v->data(), s = sizes.data()](const auto &x) mutable
        { std::memcpy(p, arg_ptr(x), *s); p += *s++; };
    (..., copy(ts));
}
```

`arg_size` and `arg_ptr` are small helper functions that add support for copying
both arithmetic and range types (the original implementation also supports a few
additional parameter types).
