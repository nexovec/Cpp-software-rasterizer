
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
// ? TODO: make vertex struct
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
inline float InvLerp(float a, float b, float val)
{
    return (val - a) / (b - a);
}
#define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
void clear_screen(Back_buffer back_buffer)
{
    for (int i = 0; i < back_buffer.height; i++)
    {
        for (int ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer(ii, i) = 0xffaaff;
        }
    }
}
unsigned int pixel_program(Triangle_2D triangle, int x, int y, unsigned int color1, unsigned int color2, unsigned int color3)
{
    // use solid color
    // return 0xff00ff00;

    // TODO: use barycentric

    // lerp color along y-axis only:
    unsigned int a1 = (color1 >> 24);
    unsigned int r1 = (color1 >> 16) & 0xff;
    unsigned int g1 = (color1 >> 8) & 0xff;
    unsigned int b1 = (color1 >> 0) & 0xff;

    unsigned int a2 = (color2 >> 24);
    unsigned int r2 = (color2 >> 16) & 0xff;
    unsigned int g2 = (color2 >> 8) & 0xff;
    unsigned int b2 = (color2 >> 0) & 0xff;

    unsigned int a3 = (color3 >> 24);
    unsigned int r3 = (color3 >> 16) & 0xff;
    unsigned int g3 = (color3 >> 8) & 0xff;
    unsigned int b3 = (color3 >> 0) & 0xff;

    unsigned int final_a = (unsigned int)Lerp(a1, a2, InvLerp(triangle.v1.y, triangle.v2.y, y));
    unsigned int final_r = (unsigned int)Lerp(r1, r2, InvLerp(triangle.v1.y, triangle.v2.y, y));
    unsigned int final_g = (unsigned int)Lerp(g1, g2, InvLerp(triangle.v1.y, triangle.v2.y, y));
    unsigned int final_b = (unsigned int)Lerp(b1, b2, InvLerp(triangle.v1.y, triangle.v2.y, y));

    // return ((final_a&0xff) << 24) + ((final_r&0xff) << 16) + ((final_g&0xff) << 8) + final_b&0xff; // <- DEBUG
    return (final_a << 24) + (final_r << 16) + (final_g << 8) + final_b;
}
void GameUpdateAndRender(Back_buffer back_buffer)
{
    // NOTE: backbuffer format is ARGB
    // NOTE: backbuffer.width is scene_width; backbuffer_height is scene_height
    clear_screen(back_buffer);

    // SECTION: generate sample triangle
    Vec_2f x = {200.0f, 400.0f};
    Vec_2f y = {500.0f, 200.0f};
    Vec_2f z = {350.0f, 350.0f};
    Triangle_2D triangle = {x, y, z};
    // Triangle_2D<float> triangle = {{200.0f, 200.0f}, {500.0f, 200.0f}, {200.0f, 500.0f}};

    // SECTION: rasterize triangle
    // TODO: rasterize one triangle
    {
        // TODO: render wireframe
        int scanline_x_start[scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
        Vec_2f *vertices = (Vec_2f *)&triangle;
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
            Vec_2f left_vertex = v1;
            Vec_2f right_vertex = v2;
            if (v1.x > right_vertex.x)
            {
                right_vertex = v2;
                left_vertex = v1;
            }
            // ? TODO: benchmark copying of data vs. pointers vs. vertex layouts etc.
            // TODO: optimize by better type choices
            for (int y = (int)lower_vertex.y; y < (int)higher_vertex.y; y++)
            {
                int relative_y_diff = y - (int)lower_vertex.y;
                float lerp_unit = (float)1 / (higher_vertex.y - lower_vertex.y);
                int x = (int)Lerp(lower_vertex.x, higher_vertex.x, lerp_unit * (float)relative_y_diff);
                // ! TODO: ignore scanlines outside of screenspace
                if (scanline_x_start[y])
                {
                    // this row has scanline boundary cached for this triangle
                    int lower_x_bound;
                    int higher_x_bound;
                    if (scanline_x_start[y] > x)
                    {
                        lower_x_bound = x;
                        higher_x_bound = scanline_x_start[y];
                    }
                    else
                    {
                        higher_x_bound = x;
                        lower_x_bound = scanline_x_start[y];
                    }
                    for (int x = lower_x_bound; x < higher_x_bound; x++)
                    {
                        back_buffer(x, y) = pixel_program(triangle, x, y, 0xffff00ff, 0xffffff00, 0xffffffff);
                    }
                    scanline_x_start[y] = 0;
                }
                else
                {
                    // this row doesn't have scanline boundary cached for this triangle
                    scanline_x_start[y] = x;
                }
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