#include <bit>
#include <cmath>
#include <cstdint>

using i32 = std::int32_t;
using u32 = std::uint32_t;

template<float f0>
constexpr float test(void) {
    constexpr float f = f0;
    constexpr auto uf = std::bit_cast<u32>(f);
    constexpr i32 s = uf >> 31; // (bool)signbit(f)
    constexpr i32 e = ((uf >> 23) & 0xff) - 127; // ilogbf(f)
    constexpr i32 m = uf & 0x7fffff;
    static_assert(s == static_cast<bool>(std::signbit(f)));
    static_assert(e == std::ilogbf(f));
    constexpr float fs = 1 - (s << 1);
    constexpr float fe = 1 << e; // scalbnf(1, e)
    constexpr float fm = 1 + m / 0x1p23f; // scalbnf(m, -23)
    static_assert(fs == std::copysign(1, f));
    static_assert(fe == scalbnf(1, e));
    static_assert(fm == 1 + scalbnf(m, -23));
    return fs * fe * fm;
}

constexpr float f0 = 42.43f;
static_assert(f0 == test<f0>());
