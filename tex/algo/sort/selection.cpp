#include <algorithm>
#include <cassert>
#include <iterator>

#include "../fundamentals/utils.hpp"

template<std::permutable I, std::sentinel_for<I> S>
constexpr void selection_sort(I b, S e) {
    assert(check_range(b, e));
    for(auto i = b; i != e; ++i) {
        assert(std::is_sorted(b, i));
        if(auto m = min_element(i, e); m != i)
            std::iter_swap(i, m);
        assert(std::is_partitioned(
            b, e, [i](const auto &x) { return x < *i; }));
    }
    assert(std::is_sorted(b, e));
}
