#include "common_defines.hpp"
#include "platform_layer.hpp"
#include "super_math.hpp"

Vec_2f Vec_2f::operator+(const Vec_2f &other)
{
    return Vec_2f{x + other.x, y + other.y};
}
Vec_2f Vec_2f::operator-(const Vec_2f &other)
{
    return Vec_2f{x - other.x, y - other.y};
}
Vec_2f Vec_2f::operator*(real32 other) const
{
    return {this->x * other, this->y * other};
}
real32 Vec_2f::operator*(Vec_2f other) const
{
    // NOTE: computes dot product
    return other.x * (this->x) + other.y * (this->y);
}
constexpr Vec_2f Vec_2f::DOWN()
{
    // TODO: test this is 0 cost
    return {-1., 0.};
}
constexpr Vec_2f Vec_2f::UP()
{
    // TODO: test this is 0 cost
    return {1., 0.};
}
constexpr Vec_2f Vec_2f::LEFT()
{
    // TODO: test this is 0 cost
    return {0., -1.};
}
constexpr Vec_2f Vec_2f::RIGHT()
{
    // TODO: test this is 0 cost
    return {0., 1.};
}

Vec_4f Mat_4x4f::operator*(Vec_4f other)
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
Mat_4x4f *Mat_4x4f::transposed()
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
Mat_4x4f *Mat_4x4f::in_place_transpose()
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
Mat_4x4f Mat_4x4f::zero()
{
    // TODO: test
    return {};
}
Mat_4x4f Mat_4x4f::unitMatrix()
{
    // TODO: test
    return {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.};
}