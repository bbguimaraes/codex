#ifndef CODEX_SORT_SELECTION_H
#define CODEX_SORT_SELECTION_H

#include <algorithm>
#include <cassert>

namespace codex {

template<std::input_iterator I, std::sentinel_for<I> S>
void check_range(I b, S e) {
    if constexpr(!std::random_access_iterator<I>)
        return;
    else if constexpr(!std::random_access_iterator<S>)
        return;
    else
        assert(b <= e);
}

template<std::forward_iterator I, std::sentinel_for<I> S>
I min_element(I b, S e) {
    check_range(b, e);
    if(b == e)
        return e;
    auto ret = b;
    for(auto i = std::next(ret); i != e; ++i) {
        assert(std::min_element(b, i) == ret);
        if(*i < *ret)
            ret = i;
    }
    assert(std::min_element(b, e) == ret);
    return ret;
}

template<std::permutable I, std::sentinel_for<I> S>
void selection_sort(I b, S e) {
    check_range(b, e);
    for(auto i = b; i != e; ++i) {
        assert(std::is_sorted(b, i));
        std::iter_swap(i, min_element(i, e));
        assert(std::is_partitioned(
            b, e, [i](const auto &x) { return x < *i; }));
    }
    assert(std::is_sorted(b, e));
}

}

#endif
