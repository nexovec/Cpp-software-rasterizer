#include "common_defines.h"
#include "main.h"
#include <cmath>
// FIXME: abstract math into a separate file
struct Vec2f
{
    float x;
    float y;
    Vec2f operator+(const Vec2f &other)
    {
        return Vec2f{x + other.x, y + other.y};
    };
    Vec2f operator-(const Vec2f &other)
    {
        return Vec2f{x - other.x, y - other.y};
    };
};
struct Triangle2D
{
    Vec2f v1;
    Vec2f v2;
    Vec2f v3;
};
template <typename T>
T sgn(T, T = 0);
template <typename T>
internal inline T sgn(T n, T zero)
{
    return (n > 0) - (n < 0);
}
internal inline float lerp(float a, float b, float ratio)
{
    return a + ratio * (b - a);
}
internal inline float invLerp(float a, float b, float val)
{
    return (val - a) / (b - a);
}
internal void clearScreen(BackBuffer back_buffer)
{
#define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
    for (int i = 0; i < back_buffer.height; i++)
    {
        for (int ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer(ii, i) = 0xffaaff;
        }
    }
}
internal inline unsigned int interpolatedColor(Triangle2D triangle, float x, float y, unsigned int color1, unsigned int color2, unsigned int color3)
{
    // use solid color
    // return 0xff00ff00;
    Vec2f v1 = triangle.v1;
    Vec2f v2 = triangle.v2;
    Vec2f v3 = triangle.v3;
    // get barycentric coordinates
    float det_t = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);
    float lam_1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / det_t;
    float lam_2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / det_t;
    float lam_3 = 1 - lam_2 - lam_1;

    unsigned char a1 = (color1 >> 24);
    unsigned char r1 = (color1 >> 16);
    unsigned char g1 = (color1 >> 8);
    unsigned char b1 = (color1 >> 0);

    unsigned char a2 = (color2 >> 24);
    unsigned char r2 = (color2 >> 16);
    unsigned char g2 = (color2 >> 8);
    unsigned char b2 = (color2 >> 0);

    unsigned char a3 = (color3 >> 24);
    unsigned char r3 = (color3 >> 16);
    unsigned char g3 = (color3 >> 8);
    unsigned char b3 = (color3 >> 0);

    unsigned char final_a = a1 * lam_1 + a2 * lam_2 + a3 * lam_3;
    unsigned char final_r = r1 * lam_1 + r2 * lam_2 + r3 * lam_3;
    unsigned char final_g = g1 * lam_1 + g2 * lam_2 + g3 * lam_3;
    unsigned char final_b = b1 * lam_1 + b2 * lam_2 + b3 * lam_3;

    // return ((final_a&0xff) << 24) + ((final_r&0xff) << 16) + ((final_g&0xff) << 8) + final_b&0xff; // <- DEBUG
    return (final_a << 24) + (final_r << 16) + (final_g << 8) + final_b;
}
internal void rasterizeTriangle(BackBuffer back_buffer, Triangle2D *triangle_ptr = 0)
{
    // SECTION: generate sample triangle
    Triangle2D triangle;
    Vec2f x_vert = {200.0f, 250.0f};
    Vec2f y_vert = {500.0f, 200.0f};
    Vec2f z_vert = {350.0f, 350.0f};
    triangle = {x_vert, y_vert, z_vert};
    if (triangle_ptr)
        triangle = *triangle_ptr;

    // TODO: render wireframe
    // SECTION: rasterize triangle
    int scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
    Vec2f *vertices = (Vec2f *)&triangle;
    for (int i1 = 0; i1 < 3; i1++)
    {
        int i2 = (i1 + 1) % 3;
        Vec2f v1 = vertices[i1];
        Vec2f v2 = vertices[i2];
        Vec2f lower_vertex = v1;
        Vec2f higher_vertex = v2;
        if (v1.y > v2.y)
        {
            lower_vertex = v2;
            higher_vertex = v1;
        }
        // ? TODO: benchmark copying of data vs. pointers vs. vertex layouts etc.
        // TODO: optimize by better type choices
        for (int y = (int)lower_vertex.y; y < (int)higher_vertex.y; y++)
        {
            float relative_y_diff = y - lower_vertex.y;
            float lerp_unit = 1.0f / (higher_vertex.y - lower_vertex.y);
            float x_bound = lerp(lower_vertex.x, higher_vertex.x, lerp_unit * relative_y_diff);
            // ! TODO: ignore scanlines outside of screenspace
            if (scanline_x_start[y])
            {
                // this row has scanline boundary cached for this triangle
                int condition = scanline_x_start[y] > x_bound;
                int lower_x_bound = condition * x_bound + !condition * scanline_x_start[y];
                int higher_x_bound = condition * scanline_x_start[y] + !condition * x_bound;
                for (int x = lower_x_bound; x < higher_x_bound; x++)
                {
                    back_buffer(x, y) = interpolatedColor(triangle, x, y, 0xffff00ff, 0xffffff00, 0xffffffff);
                }
                scanline_x_start[y] = 0;
            }
            else
            {
                // this row doesn't have scanline boundary cached for this triangle
                scanline_x_start[y] = x_bound;
            }
        }
    }
}
struct Mat2x2f
{
    float rows[4];
    Vec2f operator*(const Vec2f vec)
    {
        Vec2f back;
        // TODO: investigate SIMD
        back.x = rows[0] * vec.x + rows[1] * vec.y;
        back.y = rows[2] * vec.x + rows[3] * vec.y;
        return back;
    }
    static Mat2x2f RotationMatrix(float angle)
    {
        Mat2x2f matrix = {cos(angle), -sin(angle), sin(angle), cos(angle)};
        return matrix;
    }
};
void gameUpdateAndRender(BackBuffer back_buffer)
{
    // NOTE: backbuffer format is ARGB
    // NOTE: backbuffer.width is default_scene_width; backbuffer_height is default_scene_height
    // clearScreen(back_buffer);
    // rasterizeTriangle(back_buffer);
    {
        // Triangle2D triangle = {{0.0f, 0.0f}, {1280.f, 720.f}, {1280.f, 0.f}};
        // rasterizeTriangle(back_buffer, &triangle);
        constexpr Vec2f midpoint = {640.f, 360.f};
        static Vec2f rotating_point = {480.f, 280.f};
        rotating_point = Mat2x2f::RotationMatrix(0.1) * (rotating_point - midpoint) + midpoint;
        Vec2f other{0, 0};
        Vec2f newly_generated;
        // FIXME: some vertices are barely out of screenspace
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (float)default_scene_width / 8 - 1, 0}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {default_scene_width - 1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {default_scene_width - 1, (i + 1.0f) * (float)default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {0, default_scene_height - 1};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (float)default_scene_width / 8, default_scene_height - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }
        // FIXME: investigate why you can't use 0
        other = {1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {1, (i + 1.0f) * default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2);
            Triangle2D triangle = {other, newly_generated, rotating_point};
            rasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }
    }
    // TODO: rasterize multiple triangles
    // TODO: make a vertex buffer
    // TODO: use solid triangle colors
    // TODO: use color gradients for triangles
    // TODO: load textures
    // TODO: use textures
    // TODO: 2D AABB(+rects) physics
    // TODO: 2D GJK
    // TODO: sample scene
    // TODO: framebuffers
    // TODO: generate 3D models
    // TODO: orthographic projection
    // TODO: perspective projection
    // TODO: generate normals
    // TODO: point lights
    // TODO: ambient lights
    // TODO: generate mipmaps
    // TODO: use mipmaps
    // TODO: load obj models
    // TODO: sample scene
    // ? TODO: 3D AABB physics

    return;
}