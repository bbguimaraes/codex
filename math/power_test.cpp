#include "power_test.hpp"

#include "power.hpp"

namespace {

struct power_concept {
    // std::semiregular<T>
    constexpr power_concept() = default;
    constexpr power_concept(power_concept&&) = default;
    constexpr power_concept(const power_concept&) = default;
    power_concept &operator=(const power_concept&) = default;
    power_concept &operator=(power_concept&&) = default;
    // std::constructible_from<T, int>
    constexpr explicit power_concept(std::integral auto) {}
    // std::three_way_comparable<T>
    constexpr auto operator<=>(const power_concept&) const = default;
    // arithmetic<T>
    constexpr power_concept operator+(power_concept) { return {}; }
    constexpr power_concept operator-(power_concept) { return {}; }
    constexpr power_concept operator*(power_concept) { return {}; }
    constexpr power_concept operator/(power_concept) { return {}; }
    constexpr power_concept operator%(power_concept) { return {}; }
    constexpr power_concept operator+=(power_concept) { return {}; }
    constexpr power_concept operator-=(power_concept) { return {}; }
    constexpr power_concept operator*=(power_concept) { return {}; }
    constexpr power_concept operator/=(power_concept) { return {}; }
    constexpr power_concept operator%=(power_concept) { return {}; }
};

}

template power_concept power<power_concept>(power_concept, power_concept);

void PowerTest::power_data(void) {
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::addColumn<int>("p");
    QTest::newRow("0 0") << 0 << 0 << 1;
    QTest::newRow("0 1") << 0 << 1 << 0;
    QTest::newRow("1 0") << 1 << 0 << 1;
    QTest::newRow("1 1") << 1 << 1 << 1;
    QTest::newRow("1 2") << 1 << 2 << 1;
    QTest::newRow("2 0") << 2 << 0 << 1;
    QTest::newRow("2 1") << 2 << 1 << 2;
    QTest::newRow("2 2") << 2 << 2 << 4;
    QTest::newRow("2 3") << 2 << 3 << 8;
}

void PowerTest::power(void) {
    QFETCH(const int, x);
    QFETCH(const int, y);
    QFETCH(const int, p);
    QCOMPARE(::power(x, y), p);
}

QTEST_MAIN(PowerTest)
