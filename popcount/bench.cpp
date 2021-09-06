#include "bench.hpp"

#include <random>
#include <sstream>

extern "C" {
#include "popcount.h"
}

using popcount_fn = u32(u32);

Q_DECLARE_METATYPE(popcount_fn*);

void PopcountBench::initTestCase() {
    auto rnd = std::mt19937{std::random_device{}()};
    auto dist = std::uniform_int_distribution<u32>{};
    this->v.reserve(PopcountBench::N);
    this->expected.reserve(PopcountBench::N);
    for(std::size_t i = 0; i < PopcountBench::N; ++i)
        this->expected.emplace_back(
            static_cast<std::uint8_t>(
                __builtin_popcount(this->v.emplace_back(dist(rnd)))));
}

void PopcountBench::bench_data() {
    constexpr std::array fns = {
        popcount0, popcount1, popcount2, popcount3, popcount4, popcount5,
        popcount6, popcount7, popcount8, popcount9, popcount10,
    };
    QTest::addColumn<popcount_fn*>("f");
    std::stringstream ss = {};
    for(std::size_t i = 0; i < fns.size(); ++i) {
        ss.rdbuf()->pubseekoff(0, std::ios_base::beg);
        ss << "popcount" << i;
        QTest::newRow(ss.str().c_str()) << fns[i];
    }
}

void PopcountBench::bench() {
    QFETCH(const popcount_fn *const, f);
    const auto n = this->v.size();
    QBENCHMARK {
        for(std::size_t i = 0; i < n; ++i)
            QCOMPARE(f(this->v[i]), this->expected[i]);
    }
}

QTEST_MAIN(PopcountBench)
