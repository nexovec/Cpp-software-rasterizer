
#include "common_defines.h"
#include "main.h"
struct Vec_2f
{
    float x;
    float y;
};
struct Triangle_2D
{
    Vec_2f v1;
    Vec_2f v2;
    Vec_2f v3;
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
    Vec_2f x = {200.0f, 400.0f};
    Vec_2f y = {500.0f, 200.0f};
    Vec_2f z = {350.0f, 350.0f};
    Triangle_2D triangle = {x, y, z};
    // Triangle_2D<float> triangle = {{200.0f,200.0f},{500.0f,200.0f},{200.0f,500.0f}};
    // TODO: rasterize one triangle
    {
#define buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
        // TODO: render wireframe
        // NOTE: limits resolution to 4096 max along any dimension
        int scanline_limits[8192]; // should probably be common for all triangles, should use byte[]
        Vec_2f *vertices = (Vec_2f *)&triangle;
        Vec_2f lowest_vertex = vertices[0];
        Vec_2f highest_vertex = vertices[0];
        for (int i1 = 0; i1 < 3; i1++)
        {
            int i2 = (i1 + 1) % 3;
            Vec_2f v1 = vertices[i1];
            Vec_2f v2 = vertices[i2];
            Vec_2f lower_vertex = v1;
            Vec_2f higher_vertex = v2;
            if (v1.y > v2.y)
            {
                lower_vertex = v2;
                higher_vertex = v1;
            }
            if (lower_vertex.y < lowest_vertex.y)
            {
                lowest_vertex = lower_vertex;
            }
            if (highest_vertex.y < higher_vertex.y)
            {
                highest_vertex = higher_vertex;
            }
            // TODO: make buffer for scanline boundaries
            for (int y = lower_vertex.y; y < higher_vertex.y; y++)
            {
                float lerp_unit = (float)1 / (higher_vertex.y - lower_vertex.y);
                int x = Lerp(lower_vertex.x, higher_vertex.x, lerp_unit * (y - lower_vertex.y));
                buffer(x, y) = 0;
                // cache where scanlines begin and end
                if (scanline_limits[y * 2])
                {
                    // NOTE: set next element to -1 to say scanline_limits has been written to at that position
                    if (scanline_limits[y * 2 + 1] == -1)
                    {
                        // this row has been written to
                        // ? TODO: do rendering here directly?
                        if (scanline_limits[y * 2] > x)
                        {
                            scanline_limits[y * 2 + 1] = scanline_limits[y * 2];
                            scanline_limits[y * 2] = x;
                        }
                        else
                        {
                            scanline_limits[y * 2 + 1] = x;
                        }
                    }
                    else
                    {
                        // this row has not been written to yet
                        scanline_limits[y * 2 + 1] = -1;
                        scanline_limits[y * 2] = x;
                    }
                }
            }
            // TODO: fill triangle with solid color
        }
        for (int y = lowest_vertex.y; y < highest_vertex.y; y++)
        {
            int min_x = scanline_limits[y * 2];
            int max_x = scanline_limits[y * 2 + 1];
            for (int x = min_x; x < max_x; x++)
            {
                buffer(x, y) = 0xff00ff00;
            }
        }
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