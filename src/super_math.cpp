#include "common_defines.hpp"
#include "platform_layer.hpp"
#include "super_math.hpp"
#include <cmath> // FIXME: reconsider

vec2_f vec2_f::operator+(const vec2_f &other)
{
    return vec2_f{x + other.x, y + other.y};
}
vec2_f vec2_f::operator-(const vec2_f &other)
{
    return vec2_f{x - other.x, y - other.y};
}
vec2_f vec2_f::operator*(real_32 other) const
{
    return {this->x * other, this->y * other};
}
real_32 vec2_f::operator*(vec2_f other) const
{
    // NOTE: computes dot product
    return other.x * (this->x) + other.y * (this->y);
}
constexpr vec2_f vec2_f::DOWN()
{
    // TODO: test this is 0 cost
    return {-1., 0.};
}
constexpr vec2_f vec2_f::UP()
{
    // TODO: test this is 0 cost
    return {1., 0.};
}
constexpr vec2_f vec2_f::LEFT()
{
    // TODO: test this is 0 cost
    return {0., -1.};
}
constexpr vec2_f vec2_f::RIGHT()
{
    // TODO: test this is 0 cost
    return {0., 1.};
}

vec4_f mat4_f::operator*(vec4_f other)
{
    // TODO: test
    // TODO: use SIMD
    vec4_f result = {};
    float *res_unpacked = reinterpret_cast<float *>(&result);
    for (int_32 y = 0; y < 4; y++)
    {
        for (int_32 x = 0; x < 4; x++)
        {
            res_unpacked[y * 4 + x] += this->row_aligned_elems[y * 4 + x] * (reinterpret_cast<float *>(&other))[x];
        }
    }
    return result;
}
mat4_f *mat4_f::transposed()
{
    // NOTE: returns a transposed matrix
    // TODO: test
    mat4_f result;
    for (int_32 x = 0; x < 4; x++)
    {
        for (int_32 y = 0; y < 4; y++)
        {
            // TODO: swap more intelligently?
            result.row_aligned_elems[y * 4 + x] = this->row_aligned_elems[x * 4 + y];
            result.row_aligned_elems[x * 4 + y] = this->row_aligned_elems[y * 4 + x];
        }
    }
    return this;
}
mat4_f *mat4_f::in_place_transpose()
{
    // NOTE: transposes the matrix in-place and returns a pointer to it.
    // TODO: test
    // TODO: transposed() (without side effects on the original matrix)
    // PERFORMANCE: in_place_transpose() vs transposed() perf test
    for (int_32 y = 0; y < 4; y++)
    {
        for (int_32 x = y; x < 4; x++)
        {
            // TODO: swap more intelligently?
            real_32 holder = this->row_aligned_elems[y * 4 + x];
            this->row_aligned_elems[y * 4 + x] = this->row_aligned_elems[y * 4 + x];
            this->row_aligned_elems[x * 4 + y] = holder;
        }
    }
    return this;
}
constexpr mat4_f mat4_f::zero_matrix()
{
    // TODO: test
    return {};
}
constexpr mat4_f mat4_f::unit_matrix()
{
    // TODO: test
    return {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.};
}
mat4_f mat4_f::rotation_matrix(vec4_f)
{
    // ! TODO: implement
    return mat4_f::zero_matrix();
}
mat4_f mat4_f::translation_matrix(vec4_f translation)
{
    // ! TODO: implement
    return mat4_f::zero_matrix();
}
mat4_f ortho_projection_matrix(const vec4_f vec)
{
    // ! TODO: implement
    return mat4_f::zero_matrix();
}
mat4_f perspective_projection_matrix(const vec4_f vec)
{
    // ! TODO: implement
    return mat4_f::zero_matrix();
}

vec2_f mat2_f::operator*(const vec2_f vec)
{
    vec2_f back;
    // TODO: investigate SIMD
    back.x = row_aligned_elems[0] * vec.x + row_aligned_elems[1] * vec.y;
    back.y = row_aligned_elems[2] * vec.x + row_aligned_elems[3] * vec.y;
    return back;
}
constexpr mat2_f mat2_f::zero_matrix()
{
    return {};
}
constexpr mat2_f mat2_f::unit_matrix()
{
    return {1.0f, 0.0f, 0.0f, 1.0f};
}
mat2_f mat2_f::rotation_matrix(real_32 angle)
{
    mat2_f matrix = {cos(angle), -sin(angle), sin(angle), cos(angle)};
    return matrix;
}