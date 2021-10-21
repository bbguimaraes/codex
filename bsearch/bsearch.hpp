#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <ranges>

namespace codex {

template<std::random_access_iterator I, std::sentinel_for<I> S>
I lower_bound(I b, S e, const auto &x) {
    assert(std::is_sorted(b, e));
    [[maybe_unused]] const I ib = b;
    [[maybe_unused]] const S ie = e;
    while(b != e) {
        assert(ib <= b && b <= e && e <= ie);
        assert(std::find(ib, b, x) == b);
        assert(b == ib || b[-1] < x);
        assert(e == ie || !(*e < x));
        const I m = b + static_cast<std::size_t>(e - b) / 2;
        assert(b <= m && m < e);
        if(*m < x)
            b = m + 1;
        else
            e = m;
    }
    assert(ib <= b && b <= ie);
    assert(b == ie || !(*b < x));
    assert(b == ib || b[-1] < x);
    assert(b == ie || *b == x || std::find(ib, ie, x) == ie);
    return b;
}

template<std::random_access_iterator I, std::sentinel_for<I> S>
bool bsearch(I b, S e, const auto &x) {
    b = lower_bound(b, e, x);
    return b != e && !(x < *b);
}

}
