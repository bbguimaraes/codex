#include "rotate.hpp"

#include <numbers>

namespace {

constexpr auto a = 45.0f / 180.0f * std::numbers::pi_v<float>;
constexpr auto sin_a = std::numbers::sqrt2_v<float> / 2.0f, cos_a = sin_a;
constexpr codex::vec3 y = {0, 1, 0};

}

namespace codex {

vec3 f(const vec3 &v) { return rotate(v, sin_a, cos_a, y); }
vec3 g(const vec3 &v) { return rotate_y(v, sin_a, cos_a); }

}
