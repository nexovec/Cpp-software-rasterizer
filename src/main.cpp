#include "common_defines.hpp"
#include "asset_API.hpp"
#include "super_math.hpp"
#include "demos_2D.hpp"
#include "demos_3D.hpp"
#include "main.hpp"

assets asset_cache = assets();

internal void clear_screen(argb_texture back_buffer)
{
#define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
    for (uint_32 i = 0; i < back_buffer.height; i++)
    {
        for (uint_32 ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer(ii, i) = 0xffaaff;
        }
    }
}



struct cube_3D {
    vec4_f vertices[2 * 6];
};

void game_update_and_render(argb_texture back_buffer)
{
    // NOTE: backbuffer format is ARGB
    // NOTE: backbuffer.width is default_scene_width; backbuffer_height is default_scene_height

    // 2D demos:

    demo_draw_rotating_triangle_background(back_buffer);
    demo_draw_simple_triangle(back_buffer);

    // TODO: print file info on file load
    // TODO: custom string classes

    // render image
    {

        blt_bmp(&back_buffer, asset_cache.soldier, 50, 150);
        blt_bmp_fast(&back_buffer, asset_cache.soldier, 700, 150);
    }

    // render text to screen
    {

        // TODO: set font alpha
        // TODO: font shadows
        // TODO: font outlines
        // TODO: remove whitespace before newline/NULL characters
        asset_cache.font_1.DEBUGrender_bitmap_text(&back_buffer, (char *)"Don't be\nangry", 50, 100);
    }

    // draw texture-mapped quad
    demo_draw_texture_mapped_quad(back_buffer, asset_cache);

    // 3D demos:

    demo_render_3D_quad(back_buffer);

    // draw texture-mapped cube_3D
    // {
    //     vec4_f cube_size = vec4_f(0,0,600,0);
    //     cube_3D cube1 = generate_AA_cube_3D(cube_size, 200);
    //     DEBUGrender_cube_3D(cube1);
    //     // TODO:

    // }
    // TODO: generate 3D cube model
    // TODO: perspective projected 3D cube
    // TODO: 3D textured cube
    // TODO: Z-buffer
    // TODO: vertex depth-sort - painters algorithm
    // TODO: 2D AABB(+rects) physics
    // TODO: 2D GJK
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