#include "string_bench.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "string.hpp"

#include "../test/common.hpp"

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(std::string_view)

namespace {

void data() {
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    QTest::addColumn<std::string>("p");
    QTest::addColumn<std::string_view>("expected");
    QTest::newRow("") << ""s << "abc"sv;
    QTest::newRow("a") << "a"s << "bc"sv;
    QTest::newRow("ab") << "ab"s << "c"sv;
    QTest::newRow("abc") << "abc"s << ""sv;
    QTest::newRow("abd") << "abd"s << "abc"sv;
    QTest::newRow("_") << "_"s << "abc"sv;
    QTest::newRow("_ab") << "_ab"s << "abc"sv;
}

template<auto f>
void test(std::vector<char> v) {
    using namespace std::string_view_literals;
    QFETCH(const std::string, p);
    QFETCH(const std::string_view, expected);
    std::string_view ret = {};
    volatile std::size_t n = {};
    QBENCHMARK {
        for(std::size_t i = 0; i < 1024 * 1024; i += 1024)
            n += f(std::string_view{v.data() + i, 1024}, p).size();
    }
    QCOMPARE(f("abc", p), expected);
}

}

void StringBench::initTestCase() {
    this->v.resize(1024 * 1024);
//    auto gen = std::mt19937{std::random_device{}()};
//    std::ranges::generate(
//        this->v,
//        [&gen] { return std::uniform_int_distribution<char>{}(gen); });
    std::ranges::fill(this->v, 'a');
}

#define T(x) \
    void StringBench::x##_data() { data(); } \
    void StringBench::x() { test<codex::x>(this->v); }
T(remove_prefix_tern)
T(remove_prefix_mul)
T(remove_prefix_c)
T(remove_prefix_c_strncmp)
#undef T

QTEST_MAIN(StringBench)
