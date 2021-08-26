
#include "common_defines.h"
#include "main.h"
template <typename T>
struct Vec3
{
    T x;
    T y;
    T z;
};
// ! TODO: make a roadmap
// ! TODO: triangle struct
void game_update_and_render(BACK_BUFFER back_buffer)
{
    // SECTION: clear
    for (int i = 0; i < back_buffer.height; i++)
    {
        for (int ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer.bits[ii + i * scene_width] = 0xffaaff;
        }
    }
    // TODO: generate sample triangle
    // TODO: rasterize one triangle
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