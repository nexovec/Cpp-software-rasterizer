#pragma once
#include <cmath> // FIXME: reconsider
template <typename T>
struct Vec2
{
    T x;
    T y;
    Vec2<T> operator+(const Vec2<T> &other)
    {
        return Vec2<T>{x + other.x, y + other.y};
    };
    Vec2<T> operator-(const Vec2<T> &other)
    {
        return Vec2<T>{x - other.x, y - other.y};
    };
};
template <typename T>
struct Vec3
{
    T x;
    T y;
    T z;
};
template <typename T>
struct Vec4
{
    T x;
    T y;
    T z;
    T w;
};
template <typename T>
struct Triangle2D // FIXME: replace with Vec3
{
    Vec2<T> v1;
    Vec2<T> v2;
    Vec2<T> v3;
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