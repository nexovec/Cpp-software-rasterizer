#pragma once
#include "common_defines.hpp"
#include <cmath> // FIXME: reconsider
struct Vec_2f
{
    real32 x;
    real32 y;
    Vec_2f operator+(const Vec_2f &other);
    Vec_2f operator-(const Vec_2f &other);
    Vec_2f operator*(real32 other) const;
    real32 operator*(Vec_2f other) const;
    constexpr static Vec_2f DOWN();
    constexpr static Vec_2f UP();
    constexpr static Vec_2f LEFT();
    constexpr static Vec_2f RIGHT();
};
// constexpr const Vec_2f Vec_2f::DOWN     = {-1.0, 0.0};
// constexpr const Vec_2f Vec_2f::RIGHT = {0.0, 1.0};

struct Vec_4ui
{
    uint32 x;
    uint32 y;
    uint32 z;
    uint32 w;
};

struct Vec_4f
{
    real32 x;
    real32 y;
    real32 z;
    real32 w;
};
struct Mat_4x4f
{
    real32 row_aligned_elems[16];
    Vec_4f operator*(Vec_4f other);
    Mat_4x4f *transposed();
    Mat_4x4f *in_place_transpose();
    static Mat_4x4f zero();
    static Mat_4x4f unitMatrix();
    // static Mat_4x4f rotationMatrix(const Vec_4f vec)
    // {
    //     // ! TODO: implement
    // }
    // static Mat_4x4f translationMatrix()
    // {
    //     // ! TODO: implement
    // }
    // static Mat_4x4f orthoProjectionMatrix(const Vec_4f vec)
    // {
    //     // ! TODO: implement
    // }
    // static Mat_4x4f perspectiveProjectionMatrix(const Vec_4f vec)
    // {
    //     // ! TODO: implement
    // }
};

struct Mat2x2f
{
    real32 row_aligned_elems[4]; // FIXME: wtf
    Vec_2f operator*(const Vec_2f vec)
    {
        Vec_2f back;
        // TODO: investigate SIMD
        back.x = row_aligned_elems[0] * vec.x + row_aligned_elems[1] * vec.y;
        back.y = row_aligned_elems[2] * vec.x + row_aligned_elems[3] * vec.y;
        return back;
    }
    // Mat2x2f operator*(const Mat_4x4f other)
    // {
    //     // TODO: test
    //     Mat2x2f result = {};
    //     for (int32 y = 0; y < 2; y++)
    //     {
    //         for (int32 x = 0; x < 2, x++)
    //         {
    //             result[y * 2 + x] = this->row_aligned_elems
    //                                 // ! TODO:
    //                                 this->
    //         }
    //     }
    //     return;
    // }
    static Mat2x2f unit()
    {
        return {};
    }
    static Mat2x2f rotationMatrix(real32 angle)
    {
        Mat2x2f matrix = {cos(angle), -sin(angle), sin(angle), cos(angle)};
        return matrix;
    }
    // static Mat2x2f translationMatrix()
    // {
    //     // ! TODO:
    //     return {};
    // }
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