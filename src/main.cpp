#include "common_defines.hpp"
#include "main.hpp"
#include <cmath>
// FIXME: abstract math into a separate file
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
T math_sgn(T, T = 0);
template <typename T>
internal inline T math_sgn(T n, T zero)
{
    return (n > 0) - (n < 0);
}
template <typename T>
internal inline T math_lerp(T a, T b, real32 ratio)
{
    return a + ratio * (b - a);
}
template <typename T>
internal inline real32 math_invLerp(T a, T b, real32 val)
{
    return (val - a) / (b - a);
}
internal void clearScreen(BackBuffer back_buffer)
{
#define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
    for (uint32 i = 0; i < back_buffer.height; i++)
    {
        for (uint32 ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer(ii, i) = 0xffaaff;
        }
    }
}
internal inline uint32 interpolatedColor(real32 lam_1, real32 lam_2, real32 lam_3, uint32 color1, uint32 color2, uint32 color3)
{
    // use solid color
    // return 0xff00ff00;

    //  ? TODO: for performance reasons, you should decompose colors per-triangle, not per-pixel, but bitwise is fast and this isn't production code anywway
    const uint8 a1 = (uint8)(color1 >> 24);
    const uint8 r1 = (uint8)(color1 >> 16);
    const uint8 g1 = (uint8)(color1 >> 8);
    const uint8 b1 = (uint8)(color1 >> 0);
    const uint8 a2 = (uint8)(color2 >> 24);
    const uint8 r2 = (uint8)(color2 >> 16);
    const uint8 g2 = (uint8)(color2 >> 8);
    const uint8 b2 = (uint8)(color2 >> 0);
    const uint8 a3 = (uint8)(color3 >> 24);
    const uint8 r3 = (uint8)(color3 >> 16);
    const uint8 g3 = (uint8)(color3 >> 8);
    const uint8 b3 = (uint8)(color3 >> 0);
    const uint8 final_a = (uint8)(a1 * lam_1 + a2 * lam_2 + a3 * lam_3);
    const uint8 final_r = (uint8)(r1 * lam_1 + r2 * lam_2 + r3 * lam_3);
    const uint8 final_g = (uint8)(g1 * lam_1 + g2 * lam_2 + g3 * lam_3);
    const uint8 final_b = (uint8)(b1 * lam_1 + b2 * lam_2 + b3 * lam_3);

    // return ((final_a&0xff) << 24) + ((final_r&0xff) << 16) + ((final_g&0xff) << 8) + final_b&0xff; // <- DEBUG
    return (final_a << 24) + (final_r << 16) + (final_g << 8) + final_b;
}

struct DEBUGTexture
{
};
// TODO:
// internal inline uint32 DEBUGtextureColor(real32 lam_1, real32 lam_2, real32 lam_3, DEBUGTexture *texture)
// {
//     return 0;
// }

internal void rasterizeTriangle(BackBuffer back_buffer, Triangle2D<real32> *triangle_ptr = 0)
{
    // SECTION: generate sample triangle
    Triangle2D<real32> triangle;
    Vec2<real32> x_vert = {200.0f, 250.0f};
    Vec2<real32> y_vert = {500.0f, 200.0f};
    Vec2<real32> z_vert = {350.0f, 350.0f};
    triangle = {x_vert, y_vert, z_vert};
    if (triangle_ptr)
        triangle = *triangle_ptr;

    // TODO: render wireframe
    // SECTION: rasterize triangles
    uint32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
    Vec2<real32> *vertices = (Vec2<real32> *)&triangle;
    for (int i1 = 0; i1 < 3; i1++)
    {
        int i2 = (i1 + 1) % 3;
        Vec2<real32> v1 = vertices[i1];
        Vec2<real32> v2 = vertices[i2];
        Vec2<real32> lower_vertex = v1;
        Vec2<real32> higher_vertex = v2;
        if (v1.y > v2.y)
        {
            lower_vertex = v2;
            higher_vertex = v1;
        }
        int32 lower_y_border = max((int32)lower_vertex.y, (int32)0);
        int32 higher_y_border = min((int32)higher_vertex.y, (int32)back_buffer.height);
        for (int32 y = lower_y_border; y < higher_y_border; y++)
        {
            real32 relative_y_diff = y - lower_vertex.y;
            real32 lerp_unit = 1.0f / (higher_vertex.y - lower_vertex.y);
            real32 x_bound = math_lerp(lower_vertex.x, higher_vertex.x, lerp_unit * relative_y_diff);
            if (scanline_x_start[y])
            {
                // this row has scanline boundary cached for this triangle
                uint32 condition = scanline_x_start[y] > x_bound;
                uint32 lower_x_bound = (uint32)(condition * x_bound + !condition * scanline_x_start[y]);
                uint32 higher_x_bound = (uint32)(condition * scanline_x_start[y] + !condition * x_bound);
                for (uint32 x = lower_x_bound; x < higher_x_bound; x++)
                {
                    Vec2<real32> vert_1 = triangle.v1;
                    Vec2<real32> vert_2 = triangle.v2;
                    Vec2<real32> vert_3 = triangle.v3;
                    // get barycentric coordinates
                    real32 det_t = (vert_2.y - vert_3.y) * (vert_1.x - vert_3.x) + (vert_3.x - vert_2.x) * (vert_1.y - vert_3.y);
                    real32 lam_1 = ((vert_2.y - vert_3.y) * (x - vert_3.x) + (vert_3.x - vert_2.x) * (y - vert_3.y)) / det_t;
                    real32 lam_2 = ((vert_3.y - vert_1.y) * (x - vert_3.x) + (vert_1.x - vert_3.x) * (y - vert_3.y)) / det_t;
                    real32 lam_3 = 1 - lam_2 - lam_1;
                    // PERFORMANCE: use gradient of the barycentric coordinates
                    // #if defined(DEBUG)
                    //                     back_buffer(x, y) = DEBUGtextureColor(lam_1, lam_2, lam_3, 0);
                    // #endif
                    back_buffer(x, y) = interpolatedColor(lam_1, lam_2, lam_3, 0x88ff00ff, 0x88ffff00, 0x88ffffff);
                }
                scanline_x_start[y] = 0;
            }
            else
            {
                // this row doesn't have scanline boundary cached for this triangle
                scanline_x_start[y] = (uint32)x_bound;
            }
        }
    }
}
struct ColoredTrianglesVertexBuffer
{
    Triangle2D<real32> positions;
    Vec3<uint32> colors;
};
struct Mat2x2f
{
    real32 rows[4];
    Vec2<real32> operator*(const Vec2<real32> vec)
    {
        Vec2<real32> back;
        // TODO: investigate SIMD
        back.x = rows[0] * vec.x + rows[1] * vec.y;
        back.y = rows[2] * vec.x + rows[3] * vec.y;
        return back;
    }
    static Mat2x2f RotationMatrix(real32 angle)
    {
        Mat2x2f matrix = {cos(angle), -sin(angle), sin(angle), cos(angle)};
        return matrix;
    }
};
void gameUpdateAndRender(BackBuffer back_buffer)
{
    // NOTE: backbuffer format is ARGB
    // NOTE: backbuffer.width is default_scene_width; backbuffer_height is default_scene_height
    { // Render one triangle
        clearScreen(back_buffer);
        rasterizeTriangle(back_buffer);
    }

    { // Render multiple triangles
        // Triangle2D triangle = {{0.0f, 0.0f}, {1280.f, 720.f}, {1280.f, 0.f}};
        // rasterizeTriangle(back_buffer, &triangle);
        // TODO: create example
        constexpr Vec2<real32> midpoint = {640.f, 360.f};
        static Vec2<real32> rotating_point = {480.f, 280.f};
        rotating_point = Mat2x2f::RotationMatrix(0.1f) * (rotating_point - midpoint) + midpoint;
        Vec2<real32> other{0, 0};
        Vec2<real32> newly_generated;
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (real32)default_scene_width / 8 - 1, 0}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D<real32> triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {default_scene_width - 1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {default_scene_width - 1, (i + 1.0f) * (real32)default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D<real32> triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {0, default_scene_height - 1};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (real32)default_scene_width / 8, default_scene_height - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D<real32> triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }
        // FIXME: investigate why you can't use 0
        other = {1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {1, (i + 1.0f) * default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2);
            Triangle2D<real32> triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }
    }
    // TODO: generate quads
    // TODO: use textures
    // TODO: 2D AABB(+rects) physics
    // TODO: 2D GJK
    // TODO: sample scene
    // TODO: framebuffers as textures
    // TODO: generate 3D models
    // TODO: orthographic projection
    // TODO: perspective projection
    // TODO: generate normals
    // TODO: ambient lights
    // TODO: load obj models
    // TODO: point lights
    // TODO: generate mipmaps
    // TODO: use mipmaps
    // TODO: sample scene
    // ? TODO: 3D AABB physics

    return;
}