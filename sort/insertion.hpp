#ifndef CODEX_SORT_INSERTION_H
#define CODEX_SORT_INSERTION_H

#include <algorithm>
#include <cassert>

namespace codex {

template<std::permutable I, std::sentinel_for<I> S>
requires std::bidirectional_iterator<I>
void insertion_sort(I b, S e) {
    for(auto i = b; i != e; ++i) {
        assert(std::is_sorted(b, i));
        for(auto p = i, np = std::prev(i); p != b && *p < *np; p = np--) {
            assert(std::is_sorted(b, p));
            assert(std::is_sorted(p, i));
            std::iter_swap(np, p);
        }
    }
    assert(std::is_sorted(b, e));
}

}

#endif
