#pragma once
#include "common_defines.hpp"

struct vec2_f
{
    real_32 x;
    real_32 y;
    vec2_f operator+(const vec2_f &other);
    vec2_f operator-(const vec2_f &other);
    vec2_f operator*(real_32 other) const;
    real_32 operator*(vec2_f other) const;
    constexpr static vec2_f DOWN();
    constexpr static vec2_f UP();
    constexpr static vec2_f LEFT();
    constexpr static vec2_f RIGHT();
};

struct vec4_ui
{
    uint_32 x;
    uint_32 y;
    uint_32 z;
    uint_32 w;
    vec4_ui operator+(const vec4_ui &other);
    vec4_ui operator-(const vec4_ui &other);
    vec4_ui operator*(int_32 other) const;
    real_32 operator*(vec4_ui &other) const;
};

struct vec4_f
{
    real_32 x;
    real_32 y;
    real_32 z;
    real_32 w;
    vec4_f operator+(const vec4_f &other);
    vec4_f operator-(const vec4_f &other);
    vec4_f operator-() const;
    vec4_f operator*(real_32 other) const;
    real_32 operator*(vec4_f &other) const;
};
struct mat4_f
{
    real_32 row_aligned_elems[16];
    vec4_f operator*(vec4_f &other) const;
    mat4_f operator*(real_32 scale) const;
    mat4_f operator*(mat4_f other) const;
    mat4_f operator-(mat4_f other) const;
    mat4_f operator-() const;
    mat4_f operator+(mat4_f other) const;
    mat4_f *transposed() const;
    mat4_f *in_place_transpose();
    constexpr static mat4_f zero_matrix();
    constexpr static mat4_f unit_matrix();
    static mat4_f ones();
    static mat4_f rotation_matrix(vec4_f& rot);
    static mat4_f rotation_matrix(real_32 x_rot, real_32 y_rot, real_32 z_rot);
    static mat4_f translation_matrix(vec4_f);
    static mat4_f ortho_projection_matrix(real_32 l, real_32 r, real_32 t, real_32 b, real_32 n, real_32 f);
    static mat4_f perspective_projection_matrix(const vec4_f vec);
};

uint_32 interpolatedColor(real_32 lam_1, real_32 lam_2, real_32 lam_3, uint_32 color1, uint_32 color2, uint_32 color3);

struct mat2_f
{
    real_32 row_aligned_elems[4]; // FIXME: There was a fixme here but I don't know why
    vec2_f operator*(const vec2_f vec);
    constexpr static mat2_f unit_matrix();
    constexpr static mat2_f zero_matrix();
    static mat2_f rotation_matrix(real_32 angle);
};

// TODO: have all of the following in one compilation unit
template <typename T>
inline T math_sgn(T n, T zero)
{
    return (n > 0) - (n < 0);
}

template <typename T>
inline T math_lerp(T a, T b, real_32 ratio)
{
    return a + ratio * (b - a);
}

template <typename T>
inline real_32 math_invLerp(T a, T b, real_32 val)
{
    return (val - a) / (b - a);
}

template <typename T>
T clamp(T in, T min, T max)
{
    // PERFORMANCE: bruh. use MMX.
    if (in < min)
        return min;
    else if (in > max)
        return max;
    else
        return in;
}

template <typename T>
T max(T a, T b)
{
    // PERFORMANCE: bruh. use MMX.
    if (a > b)
        return a;
    else
        return b;
    // return ((a > b) * a) & ((a <= b) * b);
}

template <typename T>
T min(T a, T b)
{
    // PERFORMANCE: bruh. use MMX.
    if (a <= b)
        return a;
    else
        return b;
    // return (((a <= b) * a) & ((a > b) * b));
}