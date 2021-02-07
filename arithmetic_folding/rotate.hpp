#ifndef CODEX_ARITHMETIC_FOLDING_ROTATE_HPP
#define CODEX_ARITHMETIC_FOLDING_ROTATE_HPP

#include <cmath>

namespace codex {

struct vec2 { float x, y; };
struct vec3 { float x, y, z; };

vec2 rotate(const vec2 &v, float a);

vec3 operator*(const vec3 &v, float s);
vec3 operator*(float s, const vec3 &v);
vec3 operator+(const vec3 &v0, const vec3 &v1);
vec3 operator-(const vec3 &v0, const vec3 &v1);

/** Rotates \c v around an arbitrary axis \c. */
vec3 rotate(const vec3 &v, float sin, float cos, const vec3 &n);
/** Rotates \c v around the \c y axis. */
vec3 rotate_y(const vec3 &v, float sin, float cos);

vec3 f(const vec3 &v);
vec3 g(const vec3 &v);

namespace detail {

inline float dot(const vec3 &v0, const vec3 &v1)
    { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z; }

inline vec3 cross(const vec3 &v0, const vec3 &v1) {
    return {
        v0.y * v1.z - v0.z * v1.y,
        v0.z * v1.x - v0.x * v1.z,
        v0.x * v1.y - v0.y * v1.x};
}

}

inline vec2 rotate(const vec2 &v, float sin, float cos)
    { return {v.x * cos - v.y * sin, v.x * sin + v.y * cos}; }

inline vec3 operator*(const vec3 &v0, float s)
    { return {v0.x * s, v0.y * s, v0.z * s}; }
inline vec3 operator*(float s, const vec3 &v0) { return v0 * s; }
inline vec3 operator+(const vec3 &v0, const vec3 &v1)
    { return {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z}; }
inline vec3 operator-(const vec3 &v0, const vec3 &v1)
    { return {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z}; }

inline vec3 rotate(const vec3 &v, float sin, float cos, const vec3 &n) {
    const auto proj = n * detail::dot(n, v);
    const auto d = v - proj;
    const auto rot = cos * d + sin * detail::cross(n, d);
    return proj + rot;
}

inline vec3 rotate_y(const vec3 &v, float sin, float cos) {
    const vec2 ret = rotate({v.z, v.x}, sin, cos);
    return {ret.y, v.y, ret.x};
}

}

#endif
