#include <algorithm>
#include <array>
#include <list>
#include <random>

#include "../fundamentals/utils.hpp"

template<std::forward_iterator I, std::sentinel_for<I> S>
void quick_sort(I b, S e) {
    if(b == e)
        return;
    const auto p = sort_element(b, rand_it(b, e), e);
    quick_sort(b, std::prev(p));
    quick_sort(p, e);
}

int main(void) {
    std::array<int, 8> a = {};
    std::iota(begin(a), end(a), 0);
    std::ranges::shuffle(a, rnd);
    std::list<int> l = {};
    for(auto x : a)
        l.push_back(x);
    quick_sort(begin(a), end(a));
    quick_sort(begin(l), end(l));
    return !(std::ranges::is_sorted(a) && std::ranges::is_sorted(l));
}
