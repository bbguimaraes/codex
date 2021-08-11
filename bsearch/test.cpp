#include "test.hpp"

#include <array>
#include <sstream>

extern "C" {
#include "bsearch.h"
}

using bsearch_fn = bool(int, const int*, const int*);

Q_DECLARE_METATYPE(bsearch_fn*)
Q_DECLARE_METATYPE(std::size_t)

namespace {

constexpr std::array v = {0, 1, 2, 3, 4, 5, 6, 7};
constexpr std::array fns = {bsearch0, bsearch1, bsearch2, bsearch3};

}

void BSearchTest::test_data() {
    QTest::addColumn<bsearch_fn*>("f");
    QTest::addColumn<int>("x");
    QTest::addColumn<std::size_t>("bi");
    QTest::addColumn<std::size_t>("ei");
    constexpr auto n = v.size();
    std::stringstream ss = {};
    for(std::size_t f = 0, fn = fns.size(); f < fn; ++f)
        for(int x = 0; x < static_cast<int>(n); ++x)
            for(std::size_t i = 0; i <= n; ++i)
                for(std::size_t e = i; e <= n; ++e) {
                    ss = {};
                    ss << "bsearch" << f << ' ' << x << ' ' << i << ' ' << e;
                    QTest::newRow(ss.str().c_str()) << fns[f] << x << i << e;
                }
}

void BSearchTest::test() {
    QFETCH(const bsearch_fn*, f);
    QFETCH(const int, x);
    QFETCH(const std::size_t, bi);
    QFETCH(const std::size_t, ei);
    const auto *p = v.data() + bi, *e = v.data() + ei;
    QCOMPARE(f(x, p, e), lsearch(x, p, e));
}

QTEST_MAIN(BSearchTest)
