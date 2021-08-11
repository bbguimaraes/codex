#include "bench.hpp"

#include <random>
#include <sstream>

extern "C" {
#include "bsearch.h"
}

using bsearch_fn = bool(int, const int*, const int*);

Q_DECLARE_METATYPE(bsearch_fn*)

static constexpr std::array fns = {bsearch0, bsearch1, bsearch2, bsearch3};

void BSearchBench::initTestCase() {
    const auto n = static_cast<int>(this->expected.size());
    auto rnd = std::mt19937{std::random_device{}()};
    auto dist = std::uniform_int_distribution<int>{0, n};
    this->v.resize(BSearchBench::N);
    for(auto &x : this->v)
        x = dist(rnd);
    std::ranges::sort(this->v);
    for(int i = 0; i < n; ++i)
        this->expected[static_cast<unsigned>(i)] =
            std::ranges::binary_search(this->v, i);
}

void BSearchBench::bench_data() {
    QTest::addColumn<bsearch_fn*>("f");
    std::stringstream ss = {};
    for(std::size_t f = 0, fn = fns.size(); f < fn; ++f) {
        ss = {};
        ss << "bsearch" << f;
        QTest::newRow(ss.str().c_str()) << fns[f];
    }
}

void BSearchBench::bench() {
    QFETCH(bsearch_fn *const, f);
    const auto n = this->expected.size();
    const int *const b = &*this->v.begin(), *const e = &*this->v.end();
    QBENCHMARK {
        for(std::size_t i = 0; i < n; ++i)
            QCOMPARE(f(static_cast<int>(i), b, e), this->expected[i]);
    }
}

QTEST_MAIN(BSearchBench)
