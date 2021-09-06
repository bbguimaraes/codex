#include "test.hpp"

#include <sstream>

extern "C" {
#include "popcount.h"
}

using popcount_fn = u32(u32);

Q_DECLARE_METATYPE(popcount_fn*);

void PopcountTest::test_data() {
    constexpr std::array fns = {
        popcount0, popcount1, popcount2, popcount3, popcount4, popcount5,
        popcount6, popcount7, popcount8, popcount9, popcount10,
    };
    constexpr auto v = std::to_array<u32>({
        0, 1, 2, 3, 4, 5, 6, 7, 8, u32{1} << 31, UINT32_MAX - 1, UINT32_MAX,
    });
    QTest::addColumn<popcount_fn*>("f");
    QTest::addColumn<u32>("n");
    std::stringstream ss = {};
    for(std::size_t i = 0; i < fns.size(); ++i)
        for(auto n : v) {
            ss.rdbuf()->pubseekoff(0, std::ios_base::beg);
            ss << "popcount" << i << ' ' << n;
            QTest::newRow(ss.str().c_str()) << fns[i] << n;
        }
}

void PopcountTest::test() {
    QFETCH(const popcount_fn *const, f);
    QFETCH(const u32, n);
    QCOMPARE(f(n), __builtin_popcount(n));
}

QTEST_MAIN(PopcountTest)
