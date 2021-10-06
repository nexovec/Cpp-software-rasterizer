#pragma once
#include "common_defines.hpp"
#include <cmath> // FIXME: reconsider
struct Vec_2f
{
    real32 x;
    real32 y;
    Vec_2f operator+(const Vec_2f &other)
    {
        return Vec_2f{x + other.x, y + other.y};
    }
    Vec_2f operator-(const Vec_2f &other)
    {
        return Vec_2f{x - other.x, y - other.y};
    }
    Vec_2f operator*(real32 other) const
    {
        return {this->x * other, this->y * other};
    }
    real32 operator*(Vec_2f other) const
    {
        // NOTE: computes dot product
        return other.x * (this->x) + other.y * (this->y);
    }
    constexpr static Vec_2f DOWN()
    {
        // TODO: test this is 0 cost
        return {-1., 0.};
    }
    constexpr static Vec_2f UP()
    {
        // TODO: test this is 0 cost
        return {1., 0.};
    }
    constexpr static Vec_2f LEFT()
    {
        // TODO: test this is 0 cost
        return {0., -1.};
    }
    constexpr static Vec_2f RIGHT()
    {
        // TODO: test this is 0 cost
        return {0., 1.};
    }

    // inline static const Vec_2f DOWN;
    // inline static const Vec_2f RIGHT;
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
    Vec_4f operator*(Vec_4f other)
    {
        // TODO: test
        // TODO: use SIMD
        Vec_4f result = {};
        float *res_unpacked = reinterpret_cast<float *>(&result);
        for (int32 y = 0; y < 4; y++)
        {
            for (int32 x = 0; x < 4; x++)
            {
                res_unpacked[y * 4 + x] += this->row_aligned_elems[y * 4 + x] * (reinterpret_cast<float *>(&other))[x];
            }
        }
        return result;
    }
    Mat_4x4f *transposed()
    {
        // NOTE: returns a transposed matrix
        // TODO: test
        Mat_4x4f result;
        for (int32 x = 0; x < 4; x++)
        {
            for (int32 y = 0; y < 4; y++)
            {
                // TODO: swap more intelligently?
                result.row_aligned_elems[y * 4 + x] = this->row_aligned_elems[x * 4 + y];
                result.row_aligned_elems[x * 4 + y] = this->row_aligned_elems[y * 4 + x];
            }
        }
        return this;
    }
    Mat_4x4f *in_place_transpose()
    {
        // NOTE: transposes the matrix in-place and returns a pointer to it.
        // TODO: test
        // TODO: transposed() (without side effects on the original matrix)
        // PERFORMANCE: in_place_transpose() vs transposed() perf test
        for (int32 y = 0; y < 4; y++)
        {
            for (int32 x = y; x < 4; x++)
            {
                // TODO: swap more intelligently?
                real32 holder = this->row_aligned_elems[y * 4 + x];
                this->row_aligned_elems[y * 4 + x] = this->row_aligned_elems[y * 4 + x];
                this->row_aligned_elems[x * 4 + y] = holder;
            }
        }
        return this;
    }
    static Mat_4x4f zero()
    {
        // TODO: test
        return {};
    }
    static Mat_4x4f unitMatrix()
    {
        // TODO: test
        return {
            1., 0., 0., 0.,
            0., 1., 0., 0.,
            0., 0., 1., 0.,
            0., 0., 0., 1.};
    }
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