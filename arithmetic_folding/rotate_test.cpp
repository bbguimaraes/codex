#include "rotate_test.hpp"

#include <numbers>

#include "rotate.hpp"

namespace {

constexpr auto a = 45.0f / 180.0f * std::numbers::pi_v<float>;
constexpr auto sqrt2 = std::numbers::sqrt2_v<float>;
constexpr auto sin_a = sqrt2 / 2.0f, cos_a = sin_a;
constexpr codex::vec3 v = {1, 2, 3};

}

void RotateTest::rotate() {
    const auto ret = codex::f(v);
    QCOMPARE(ret.x, 2 * sqrt2);
    QCOMPARE(ret.y, 2);
    QCOMPARE(ret.z, sqrt2);
}

void RotateTest::rotate_y() {
    const auto ret = codex::g(v);
    QCOMPARE(ret.x, 2 * sqrt2);
    QCOMPARE(ret.y, 2);
    QCOMPARE(ret.z, sqrt2);
}

QTEST_MAIN(RotateTest)
