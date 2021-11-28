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

// TODO: use templates and using for vectors?
vec4_f vec4_f::operator+(const vec4_f &other)
{
    return {other.x + this->x, other.y * this->y, other.z * this->z, other.w * this->w};
}
vec4_f vec4_f::operator-(const vec4_f &other)
{
    return {other.x + this->x, other.y * this->y, other.z * this->z, other.w * this->w};
}
vec4_f vec4_f::operator*(real_32 m) const
{
    return {m * this->x, m * this->y, m * this->z, m * this->w};
}
real_32 vec4_f::operator*(vec4_f &m) const
{
    return m.x * this->x + m.y * this->y + m.z * this->z + m.w * this->w;
}

vec4_ui vec4_ui::operator+(const vec4_ui &other)
{
    return {other.x + this->x, other.y * this->y, other.z * this->z, other.w * this->w};
}
vec4_ui vec4_ui::operator-(const vec4_ui &other)
{
    return {other.x + this->x, other.y * this->y, other.z * this->z, other.w * this->w};
}
vec4_ui vec4_ui::operator*(int_32 m) const
{
    return {m * this->x, m * this->y, m * this->z, m * this->w};
}
real_32 vec4_ui::operator*(vec4_ui &m) const
{
    return m.x * this->x + m.y * this->y + m.z * this->z + m.w * this->w;
}

vec4_f mat4_f::operator*(vec4_f &other)
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

mat4_f *mat4_f::transposed_matrix()
{
    // NOTE: returns a transposed_matrix matrix
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
    // TODO: transposed_matrix() (without side effects on the original matrix)
    // PERFORMANCE: in_place_transpose() vs transposed_matrix() perf test
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
    return {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.};
}
mat4_f mat4_f::rotation_matrix(vec4_f)
{
    // TODO: implement

    return mat4_f::zero_matrix();
}
mat4_f mat4_f::translation_matrix(vec4_f translation)
{
    // TODO: test
    mat4_f result = mat4_f::unit_matrix();
    result.row_aligned_elems[0 * 4 + 3] = translation.x;
    result.row_aligned_elems[1 * 4 + 3] = translation.y;
    result.row_aligned_elems[2 * 4 + 3] = translation.z;
    return result;
}
mat4_f mat4_f::ortho_projection_matrix(real_32 l, real_32 r, real_32 t, real_32 b, real_32 n, real_32 f)
{
    // TODO: test
    return {
        2 / (r - l), 0,           0,           -(r + l) / (r - l),
        0,           2 / (t - b), 0,           -(t + b) / (t - b),
        0,           0,          -2 * (f - n), -(f + n) / (f - n),
        0,           0,           0,            1
        };
}
mat4_f mat4_f::perspective_projection_matrix(const vec4_f vec)
{
    // TODO: implement
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
uint_32 interpolatedColor(real_32 lam_1, real_32 lam_2, real_32 lam_3, uint_32 color1, uint_32 color2, uint_32 color3)
{
    // use solid color
    // return 0xff00ff00;

    //  ? TODO: for performance reasons, you should decompose colors per-triangle, not per-pixel, but bitwise is fast and this won't be used anywway
    const uint_8 a1 = (uint_8)(color1 >> 24);
    const uint_8 r1 = (uint_8)(color1 >> 16);
    const uint_8 g1 = (uint_8)(color1 >> 8);
    const uint_8 b1 = (uint_8)(color1 >> 0);
    const uint_8 a2 = (uint_8)(color2 >> 24);
    const uint_8 r2 = (uint_8)(color2 >> 16);
    const uint_8 g2 = (uint_8)(color2 >> 8);
    const uint_8 b2 = (uint_8)(color2 >> 0);
    const uint_8 a3 = (uint_8)(color3 >> 24);
    const uint_8 r3 = (uint_8)(color3 >> 16);
    const uint_8 g3 = (uint_8)(color3 >> 8);
    const uint_8 b3 = (uint_8)(color3 >> 0);
    const uint_8 final_a = (uint_8)(a1 * lam_1 + a2 * lam_2 + a3 * lam_3);
    const uint_8 final_r = (uint_8)(r1 * lam_1 + r2 * lam_2 + r3 * lam_3);
    const uint_8 final_g = (uint_8)(g1 * lam_1 + g2 * lam_2 + g3 * lam_3);
    const uint_8 final_b = (uint_8)(b1 * lam_1 + b2 * lam_2 + b3 * lam_3);

    // return ((final_a&0xff) << 24) + ((final_r&0xff) << 16) + ((final_g&0xff) << 8) + final_b&0xff; // <- DEBUG
    return (final_a << 24) + (final_r << 16) + (final_g << 8) + final_b;
}