#pragma once
#include <cmath> // FIXME: reconsider
struct Vec_2f
{
    float x;
    float y;
    Vec_2f operator+(const Vec_2f &other)
    {
        return Vec_2f{x + other.x, y + other.y};
    };
    Vec_2f operator-(const Vec_2f &other)
    {
        return Vec_2f{x - other.x, y - other.y};
    };
};
struct Vec_3f
{
    float x;
    float y;
    float z;
};
struct Vec_3ui
{
    uint32 x;
    uint32 y;
    uint32 z;
};
struct Vec_4f
{
    float x;
    float y;
    float z;
    float w;
};
struct Triangle2D // FIXME: replace with Vec3
{
    Vec_2f v1;
    Vec_2f v2;
    Vec_2f v3;
};

template <typename T>
inline T math_sgn(T n, T zero)
{
    return (n > 0) - (n < 0);
}
template <typename T>
inline T math_lerp(T a, T b, real32 ratio)
{
    return a + ratio * (b - a);
}
template <typename T>
inline real32 math_invLerp(T a, T b, real32 val)
{
    return (val - a) / (b - a);
}