#include "bench.hpp"

#include <random>
#include <sstream>

#include "bsearch.hpp"

namespace {

using T = int;
using V = std::vector<int>;
using I = typename V::const_iterator;
using bsearch_fn = bool(I, I, const T&);
constexpr std::array fns = {codex::bsearch<I, I, T>, std::binary_search<I, T>};

}

Q_DECLARE_METATYPE(bsearch_fn*)

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
    const auto b = this->v.begin(), e = this->v.end();
    QBENCHMARK {
        for(std::size_t i = 0; i < n; ++i)
            QCOMPARE(f(b, e, static_cast<int>(i)), this->expected[i]);
    }
}

QTEST_MAIN(BSearchBench)
