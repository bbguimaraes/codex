// g++ -std=c++20 -masm=intel -fno-stack-protector -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables

#include <array>
#include <algorithm>
#include <random>

template<typename I>
void qsort0(I b, I e) {
    if(e <= b)
        return;
    auto m = b + (e - b) / 2;
    std::swap(*b, *m);
    m = std::partition(b + 1, e, [v = *b](auto x) { return x < v; });
    std::swap(*b, *--m);
    qsort0(b, m);
    qsort0(m + 1, e);
}

template<typename I>
void qsort1(I b, I e) {
    if(e <= b)
        return;
    const auto p = e - 1;
    auto m = std::partition(b, p, [v = *p](auto x) { return x < v; });
    std::swap(*p, *m);
    qsort1(b, m);
    qsort1(m + 1, e);
}

bool test(auto f) {
    bool ret = true;
    for(int i = 0; i < 32; ++i) {
        std::array v = {0, 1, 2, 3, 4, 5, 6, 7};
        std::ranges::shuffle(v, std::random_device{});
        f(begin(v), end(v));
        ret &= std::ranges::is_sorted(v);
    }
    return ret;
}

int main() {
    const bool ret = test(qsort0<int*>) && test(qsort1<int*>);
    return !ret;
}
