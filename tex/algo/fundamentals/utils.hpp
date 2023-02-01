#include <algorithm>
#include <iterator>

static std::random_device rnd;

constexpr auto less_than(auto y) { return [y](auto x) { return x < y; }; }

template<std::random_access_iterator I, std::sentinel_for<I> S>
constexpr bool check_range(I b, S e) {
    return 0 <= std::distance(b, e);
}

template<std::input_iterator I, std::sentinel_for<I> S>
constexpr bool check_range(I b, S e) {
    if constexpr(std::forward_iterator<I>)
        std::distance(b, e);
    return true;
}

template<std::input_iterator I, std::sentinel_for<I> S>
constexpr bool contains(I b, S e, I i) {
    return check_range(b, i) && check_range(i, e);
}

template<std::forward_iterator I, std::sentinel_for<I> S>
I rand_it(I b, S e) {
    using D = std::iter_difference_t<I>;
    const D n = std::distance(b, e);
    const D i = std::uniform_int_distribution<D>{0, n - 1}(rnd);
    return std::next(b, i);
}

template<std::input_iterator I, std::sentinel_for<I> S>
constexpr bool is_min_element(I b, S e, const auto &x) {
    return std::all_of(b, e, [&x](const auto &y) { return !(y < x); });
}

template<std::forward_iterator I, std::sentinel_for<I> S>
constexpr I min_element(I b, S e) {
    assert(check_range(b, e));
    if(b == e)
        return b;
    auto ret = b;
    for(auto i = std::next(ret); i != e; ++i) {
        assert(is_min_element(b, i, *ret));
        if(*i < *ret)
            ret = i;
    }
    assert(contains(b, e, ret));
    assert(is_min_element(b, e, *ret));
    return ret;
}

template<std::forward_iterator I, std::sentinel_for<I> S>
I sort_element(I b, I i, S e) {
    std::iter_swap(b, i);
    const auto ret = std::partition(std::next(b), e, less_than(*b));
    std::iter_swap(b, std::prev(ret));
    return ret;
}
