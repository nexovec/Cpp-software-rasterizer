#include "common_defines.hpp"
#include "asset_API.hpp"
#include "super_math.hpp"
#include "main.hpp"

assets asset_cache = assets();

struct ColoredTrianglesVertexBuffer
{
    vec2_f positions;
    vec4_ui colors;
};

struct triangle_2D
{
    vec2_f v1;
    vec2_f v2;
    vec2_f v3;
};

struct quad_2D
{
    triangle_2D bottom;
    triangle_2D bottom_uv;
    triangle_2D top;
    triangle_2D top_uv;
};

internal void clearScreen(argb_texture back_buffer)
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
internal inline uint_32 interpolatedColor(real_32 lam_1, real_32 lam_2, real_32 lam_3, uint_32 color1, uint_32 color2, uint_32 color3)
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

internal void DEBUGrasterize_triangle(argb_texture back_buffer, triangle_2D *triangle_ptr = 0)
{
    // SECTION: generate sample triangle
    triangle_2D triangle;
    vec2_f x_vert = {200.0f, 250.0f};
    vec2_f y_vert = {500.0f, 200.0f};
    vec2_f z_vert = {350.0f, 350.0f};
    triangle = {x_vert, y_vert, z_vert};
    if (triangle_ptr)
        triangle = *triangle_ptr;

    // TODO: render wireframe
    // SECTION: rasterize triangles
    uint_32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
    vec2_f *vertices = (vec2_f *)&triangle;
    for (int i1 = 0; i1 < 3; i1++)
    {
        int i2 = (i1 + 1) % 3;
        vec2_f v1 = vertices[i1];
        vec2_f v2 = vertices[i2];
        vec2_f lower_vertex = v1;
        vec2_f higher_vertex = v2;
        if (v1.y > v2.y)
        {
            lower_vertex = v2;
            higher_vertex = v1;
        }
        int_32 lower_y_border = max((int_32)lower_vertex.y, (int_32)0);
        int_32 higher_y_border = min((int_32)higher_vertex.y, (int_32)back_buffer.height);
        for (int_32 y = lower_y_border; y < higher_y_border; y++)
        {
            real_32 relative_y_diff = y - lower_vertex.y;
            real_32 lerp_unit = 1.0f / (higher_vertex.y - lower_vertex.y);
            real_32 x_bound = math_lerp(lower_vertex.x, higher_vertex.x, lerp_unit * relative_y_diff);
            if (scanline_x_start[y])
            {
                // this row has scanline boundary cached for this triangle
                uint_32 condition = scanline_x_start[y] > x_bound;
                uint_32 lower_x_bound = (uint_32)(condition * x_bound + (!condition) * scanline_x_start[y]);
                uint_32 higher_x_bound = (uint_32)(condition * scanline_x_start[y] + !condition * x_bound);
                for (uint_32 x = lower_x_bound; x < higher_x_bound; x++)
                {
                    vec2_f vert_1 = triangle.v1;
                    vec2_f vert_2 = triangle.v2;
                    vec2_f vert_3 = triangle.v3;
                    // get barycentric coordinates
                    real_32 det_t = (vert_2.y - vert_3.y) * (vert_1.x - vert_3.x) + (vert_3.x - vert_2.x) * (vert_1.y - vert_3.y);
                    real_32 lam_1 = ((vert_2.y - vert_3.y) * (x - vert_3.x) + (vert_3.x - vert_2.x) * (y - vert_3.y)) / det_t;
                    real_32 lam_2 = ((vert_3.y - vert_1.y) * (x - vert_3.x) + (vert_1.x - vert_3.x) * (y - vert_3.y)) / det_t;
                    real_32 lam_3 = 1 - lam_2 - lam_1;
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
                scanline_x_start[y] = (uint_32)x_bound;
            }
        }
    }
}
internal inline uint_32 DEBUGtextureColor(real_32 lam_1, real_32 lam_2, real_32 lam_3, vec2_f texel_1, vec2_f texel_2, vec2_f texel_3, argb_texture texture)
{
    // NOTE: uses brycentric coords
    // TODO: bilinear filtering
    // TODO: trilinear filtering
    int_32 final_x = (int_32)((texel_1.x * lam_1 + texel_2.x * lam_2 + texel_3.x * lam_3) + 0.5);
    int_32 final_y = (int_32)((texel_1.y * lam_1 + texel_2.y * lam_2 + texel_3.y * lam_3) + 0.5);

    return texture.bits[(final_y * texture.width) + final_x];
}
internal void rasterizeTriangleTextured(argb_texture back_buffer, triangle_2D *triangle_ptr, triangle_2D *uv_coords)
{
    // FIXME: code duplication
    triangle_2D triangle = *triangle_ptr;
    // TODO: render wireframe
    // SECTION: rasterize triangles
    real_32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
    vec2_f *vertices = (vec2_f *)&triangle;
    for (int i1 = 0; i1 < 3; i1++)
    {
        int i2 = (i1 + 1) % 3;
        vec2_f v1 = vertices[i1];
        vec2_f v2 = vertices[i2];
        vec2_f lower_vertex = v1;
        vec2_f higher_vertex = v2;
        if (v1.y > v2.y)
        {
            lower_vertex = v2;
            higher_vertex = v1;
        }
        int_32 lower_y_border = max((int_32)lower_vertex.y, (int_32)0);
        int_32 higher_y_border = min((int_32)higher_vertex.y, (int_32)back_buffer.height);
        for (int_32 y = lower_y_border; y < higher_y_border; y++)
        {
            real_32 relative_y_diff = y - lower_vertex.y;
            real_32 lerp_unit = 1.0f / (higher_vertex.y - lower_vertex.y);
            real_32 x_bound = math_lerp(lower_vertex.x, higher_vertex.x, lerp_unit * relative_y_diff);
            if (scanline_x_start[y])
            {
                // this row has scanline boundary cached for this triangle
                // PERFORMANCE: you can cache some things here and avoid excessive casts
                uint_32 lower_x = (uint_32)clamp<float>(min(x_bound, scanline_x_start[y]), 0, (real_32)(back_buffer.width - 1));
                uint_32 higher_x = (uint_32)clamp<float>(max(x_bound, scanline_x_start[y]), 0, (real_32)(back_buffer.width - 1));
                for (uint_32 x = lower_x; x < higher_x; x++)
                {
                    vec2_f vert_1 = triangle.v1;
                    vec2_f vert_2 = triangle.v2;
                    vec2_f vert_3 = triangle.v3;
                    // get barycentric coordinates
                    real_32 det_t = (vert_2.y - vert_3.y) * (vert_1.x - vert_3.x) + (vert_3.x - vert_2.x) * (vert_1.y - vert_3.y);
                    real_32 lam_1 = ((vert_2.y - vert_3.y) * (x - vert_3.x) + (vert_3.x - vert_2.x) * (y - vert_3.y)) / det_t;
                    real_32 lam_2 = ((vert_3.y - vert_1.y) * (x - vert_3.x) + (vert_1.x - vert_3.x) * (y - vert_3.y)) / det_t;
                    real_32 lam_3 = 1 - lam_2 - lam_1;
                    // PERFORMANCE: use gradient of the barycentric coordinates
                    // #if defined(DEBUG)
                    //                     back_buffer(x, y) = DEBUGtextureColor(lam_1, lam_2, lam_3, 0);
                    // #endif
                    back_buffer(x, y) = DEBUGtextureColor(lam_1, lam_2, lam_3, uv_coords->v1, uv_coords->v2, uv_coords->v3, asset_cache.soldier.getUnderlyingTexture());
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

/* Generates axis aligned quad. */
internal quad_2D generate_AA_quad(vec2_f pos, vec2_f size)
{
    triangle_2D triangleA = {pos, pos + vec2_f(1.0f, 0.0f) * size.y, pos + size};
    triangle_2D triangleA_uv = {{0.0f, 0.0f}, {512.0f, 0.0f}, {512.0f, 512.0f}};
    triangle_2D triangleB = {pos, pos + vec2_f(0.0f, 1.0f) * size.x, pos + size};
    triangle_2D triangleB_uv = {{0.0f, 0.0f}, {0.0f, 512.0f}, {512.0f, 512.0f}};
    return {triangleA, triangleA_uv, triangleB, triangleB_uv};
}
internal void DEBUGrenderQuad2D(argb_texture back_buffer, quad_2D *quad)
{
    triangle_2D *as_array = (triangle_2D *)quad;
    for (int i = 0; i < 2; i++)
    {
        rasterizeTriangleTextured(back_buffer, as_array + i * 2, as_array + i * 2 + 1);
    }
}
void gameUpdateAndRender(argb_texture back_buffer)
{
    // NOTE: backbuffer format is ARGB
    // NOTE: backbuffer.width is default_scene_width; backbuffer_height is default_scene_height

    {
        // Render one triangle

        clearScreen(back_buffer);
        DEBUGrasterize_triangle(back_buffer);
    }

    {
        // Render multiple triangles

        // triangle_2D triangle = {{0.0f, 0.0f}, {1280.f, 720.f}, {1280.f, 0.f}};
        // DEBUGrasterize_triangle(back_buffer, &triangle);

        // TODO: create example
        constexpr vec2_f midpoint = {640.f, 360.f};

        // FIXME: don't use persistent
        persistent vec2_f rotating_point = {480.f, 280.f};
        rotating_point = mat2_f::rotation_matrix(0.1f) * (rotating_point - midpoint) + midpoint;
        vec2_f other{0, 0};
        vec2_f newly_generated;
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (real_32)default_scene_width / 8 - 1, 0}; //+((rand() % default_scene_width)-default_scene_width/2)
            triangle_2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterize_triangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {default_scene_width - 1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {default_scene_width - 1, (i + 1.0f) * (real_32)default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            triangle_2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterize_triangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {0, default_scene_height - 1};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (real_32)default_scene_width / 8, default_scene_height - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            triangle_2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterize_triangle(back_buffer, &triangle);
            other = newly_generated;
        }
        // FIXME: investigate why you can't use 0
        other = {1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {1, (i + 1.0f) * default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2);
            triangle_2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterize_triangle(back_buffer, &triangle);
            other = newly_generated;
        }
    }

    // TODO: print file info on file load
    // TODO: custom string classes
    // #ifdef DEBUG
    //     int_32 h = (int_32)assets.font_image.bh->bmp_info_header.Height;
    //     char buf[128];
    //     sprintf_s(buf, 128, "%ld\n", h);
    //     OutputDebugStringA(buf);
    // #endif

    // render image
    {

        blt_bmp(&back_buffer, asset_cache.soldier, 50, 150);
        blt_bmp_fast(&back_buffer, asset_cache.soldier, 700, 150);
        // font_tile_map.DEBUGdraw(&back_buffer, 6, 1, 200, 400);
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
    {

        quad_2D quad = generate_AA_quad(vec2_f(900.0, 30.0), vec2_f(400.0, 400.0));
        DEBUGrenderQuad2D(back_buffer, &quad);
    }

    // draw texture-mapped cube
    {

        // ! TODO:
    }
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