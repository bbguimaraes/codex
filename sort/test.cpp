#include "test.hpp"

#include <forward_list>

#include "insertion.hpp"
#include "selection.hpp"

namespace {

using list = std::forward_list<int>;
using value_type = list::value_type;
using iterator = list::iterator;

constexpr std::array V = {2, 1, 7, 5, 6, 0, 4, 3};
constexpr auto N = V.size();

void data(void) {
    QTest::addColumn<std::size_t>("b");
    QTest::addColumn<std::size_t>("e");
    for(std::size_t b = 0; b <= N; ++b)
        for(std::size_t e = b; e <= N; ++e)
            QTest::newRow(QString{"%1:%2"}.arg(b).arg(e).toUtf8().constData())
                << b << e;
}

template<typename T>
void test(auto sort) {
    QFETCH(const std::size_t, b);
    QFETCH(const std::size_t, e);
    T v0(begin(V), end(V));
    std::vector<int> v1(begin(V), end(V));
    const auto b0 = std::next(begin(v0), b), e0 = std::next(begin(v0), e);
    const auto b1 = std::next(begin(v1), b), e1 = std::next(begin(v1), e);
    sort(b0, e0);
    std::sort(b1, e1);
    QVERIFY(std::is_sorted(b0, e0));
    QVERIFY(std::equal(b0, e0, b1));
    v0.sort();
    std::ranges::sort(v1);
    QVERIFY(std::ranges::equal(v0, v1));
}

}

void SortTest::selection_data(void) { data(); }

void SortTest::selection(void) {
    test<std::forward_list<int>>(
        [](auto b, auto e) { codex::selection_sort(b, e); });
}

QTEST_MAIN(SortTest)
