#ifndef CODEX_SORT_SELECTION_H
#define CODEX_SORT_SELECTION_H

#include <algorithm>
#include <cassert>

namespace codex {

template<std::input_iterator I, std::sentinel_for<I> S>
constexpr bool is_range(I b, S e) {
    if constexpr(!std::random_access_iterator<I>)
        return true;
    else if constexpr(!std::random_access_iterator<S>)
        return true;
    else
        return b <= e;
}

template<std::input_iterator I, std::sentinel_for<I> S>
constexpr I min_element(I b, S e) {
    constexpr bool fwd = std::forward_iterator<I>;
    assert(is_range(b, e));
    if(b == e)
        return e;
    auto ret = b;
    for(auto i = std::next(ret); i != e; ++i) {
        if constexpr(fwd)
            assert(std::min_element(b, i) == ret);
        if(*i < *ret)
            ret = i;
    }
    assert(b <= ret && ret < e);
    if constexpr(fwd)
        assert(is_min_element(b, e, *ret));
    return ret;
}

template<std::permutable I, std::sentinel_for<I> S>
constexpr void selection_sort(I b, S e) {
    assert(is_range(b, e));
    for(auto i = b; i != e; ++i) {
        assert(std::is_sorted(b, i));
        if(auto m = min_element(i, e); m != i)
            std::iter_swap(i, m);
        assert(std::is_partitioned(
            b, e, [i](const auto &x) { return x < *i; }));
    }
    assert(std::is_sorted(b, e));
}

}

#endif
