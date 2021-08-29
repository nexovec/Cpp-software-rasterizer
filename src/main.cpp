
#include "common_defines.h"
#include "main.h"
template <typename T>
struct Vec_2
{
    T x;
    T y;
};
template <typename T> // FIXME: naaaah, bad idea
struct Triangle_2D
{
    Vec_2<T> v1;
    Vec_2<T> v2;
    Vec_2<T> v3;
};
template <typename T>
T Sgn(T, T = 0);
template <typename T>
inline T Sgn(T n, T zero)
{
    return (n > 0) - (n < 0);
}
inline float Lerp(float a, float b, float ratio)
{
    return a + ratio * (b - a);
}
// ! TODO: make a roadmap
// ! TODO: triangle struct
void GameUpdateAndRender(Back_buffer back_buffer)
{
    // SECTION: clear
    // NOTE: format is ARGB
    for (int i = 0; i < back_buffer.height; i++)
    {
        for (int ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer.bits[ii + i * scene_width] = 0xffaaff;
        }
    }
    // SECTION: generate sample triangle
    Vec_2<float> x = {200.0f, 400.0f};
    Vec_2<float> y = {500.0f, 200.0f};
    Vec_2<float> z = {350.0f, 350.0f};
    Triangle_2D<float> triangle = {x, y, z};
    // Triangle_2D<float> triangle = {{200.0f,200.0f},{500.0f,200.0f},{200.0f,500.0f}};
    // TODO: rasterize one triangle
    {
        // TODO: render wireframe
        int v12_y_diff = triangle.v1.y - triangle.v2.y;
        int sgn = Sgn(v12_y_diff);
        for (int y_diff = 0; y_diff < v12_y_diff; y_diff += sgn) // FIXME: comparison
        {
            int x = Lerp(triangle.v1.x, triangle.v2.x, ((float)1 / v12_y_diff) * y_diff);
            int y = Lerp(triangle.v1.y, triangle.v2.y, ((float)1 / v12_y_diff) * y_diff);

            back_buffer.bits[back_buffer.width * y + x] = 0xff000000;
        }
        int v23_y_diff = triangle.v2.y - triangle.v3.y;
        sgn = Sgn(v23_y_diff);
        for (int y_diff = 0; y_diff > v23_y_diff; y_diff += sgn) // FIXME: comparison
        {
            int x = Lerp(triangle.v2.x, triangle.v3.x, ((float)1 / v23_y_diff) * y_diff);
            int y = Lerp(triangle.v2.y, triangle.v3.y, ((float)1 / v23_y_diff) * y_diff);

            back_buffer.bits[back_buffer.width * y + x] = 0xff000000;
        }
        int v13_y_diff = triangle.v1.y - triangle.v3.y;
        sgn = Sgn(v13_y_diff);
        for (int y_diff = 0; y_diff < v13_y_diff; y_diff += sgn) // FIXME: comparison
        {
            int x = Lerp(triangle.v1.x, triangle.v3.x, ((float)1 / v13_y_diff) * y_diff);
            int y = Lerp(triangle.v1.y, triangle.v3.y, ((float)1 / v13_y_diff) * y_diff);

            back_buffer.bits[back_buffer.width * y + x] = 0xff000000;
        }
        // for (int y = triangle.v1.y; y < triangle.v1.y + v12_y_diff; y++)
        // {
        //     back_buffer.bits[scene_width * y + triangle.v1.x] = 0x00000000;
        //     back_buffer.bits[scene_width * y + triangle.v1.x + 1] = 0x00000000;
        //     back_buffer.bits[scene_width * y + triangle.v1.x + 2] = 0x00000000;
        //     back_buffer.bits[scene_width * y + triangle.v1.x + 3] = 0x00000000;
        //     back_buffer.bits[scene_width * y + triangle.v1.x + 4] = 0x00000000;
        //     back_buffer.bits[scene_width * y + triangle.v1.x + 5] = 0x00000000;
        // }
    }
    // TODO: generate triangles that fill screen
    // TODO: rasterize multiple triangles
    // TODO: make a vertex buffer
    // TODO: use solid triangle colors
    // TODO: use color gradients for triangles
    // TODO: load textures
    // TODO: use textures
    // TODO: 2D AABB(+rects) physics
    // TODO: 2D GJK
    // TODO: sample scene
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