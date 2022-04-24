#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <span>
#include <utility>

#define FWD(x) std::forward<decltype(x)>(x)

namespace detail {

struct entry {
    const void *b, *e;
    void *r;
    void (*f)(void*, void*, void*);
    template<auto f, std::ranges::contiguous_range R>
    static entry make(R &r);
};

template<auto f, std::ranges::contiguous_range R>
auto entry::make(R &r) -> entry {
    return {
        .b = &*r.cbegin(),
        .e = &*r.cend(),
        .r = &r,
        .f = [](void *d, void *rp, void *vp) {
            return f(
                d, static_cast<R*>(rp),
                static_cast<std::ranges::range_value_t<R>*>(vp));
        },
    };
}

template<typename R, typename T>
using same_constness = std::conditional_t<std::is_const_v<R>, const T, T>;

constexpr bool contains(
    const std::ranges::contiguous_range auto &r, const auto &x
) {
    const void *const b = &*std::ranges::cbegin(r);
    const void *const e = &*std::ranges::cend(r);
    const void *const p = &x;
    return b <= p && p < e;
}

template<typename T>
auto find(std::span<const entry> s, T *p) {
    auto *const vp = static_cast<detail::same_constness<T, void>*>(p);
    auto *const cp = static_cast<detail::same_constness<T, char>*>(vp);
    const auto ret = std::ranges::lower_bound(s, vp, std::less<>{}, &entry::e);
    if(ret == end(s))
        return ret;
    if(!detail::contains(std::span<const char>{
        static_cast<const char*>(ret->b),
        static_cast<const char*>(ret->e),
    }, *cp))
        return end(s);
    return ret;
}

template<typename T>
void call(T *p, void *d, std::span<const entry> s) {
    auto *const vp = static_cast<detail::same_constness<T, void>*>(p);
    auto *const cp = static_cast<detail::same_constness<T, char>*>(vp);
    if(const auto i = detail::find(s, p); i != end(s))
        i->f(d, i->r, p);
}

}

template<std::size_t N>
class storage_switch {
public:
    template<auto f>
    static storage_switch make(
        void *d, std::ranges::contiguous_range auto &&...rs);
    template<typename T>
    void operator()(T *p) const { detail::call(p, this->d, this->entries); }
private:
    using entry = detail::entry;
    explicit storage_switch(void *d_, std::array<entry, N> a)
        : d{d_}, entries{std::move(a)} {}
    void *d;
    std::array<entry, N> entries;
};

template<auto f, std::ranges::contiguous_range ...Rs>
auto make_storage_switch(void *d, Rs &&...rs) {
    return storage_switch<sizeof...(Rs)>::template make<f>(d, FWD(rs)...);
}

template<std::size_t N>
template<auto f>
storage_switch<N> storage_switch<N>::make(
    void *d, std::ranges::contiguous_range auto &&...rs
) {
    auto ret = storage_switch{d, {entry::make<f>(FWD(rs))...}};
    std::ranges::sort(ret.entries, std::less<>{}, &entry::e);
    return ret;
}

template<typename> const char *type_name;
#define X(x) template<> const char *type_name<x> = #x;
X(int)
X(unsigned)
X(float)
#undef X

int main(void) {
    constexpr auto f = []<typename T>(void *d, const auto *v, const T *p) {
        std::cout
            << d << ' ' << (p - v->data()) << ' ' << *p << ' '
            << type_name<T> << '\n';
    };
    std::array is = {0, 1};
    std::array us = {2u, 3u, 4u};
    std::array fs = {5.0f, 6.0f, 7.0f, 8.0f};
    const auto d = reinterpret_cast<void*>(42);
    const auto s = make_storage_switch<f>(d, is, us, fs);
    s(&is[0]);
    s(&us[1]);
    s(&fs[2]);
}
