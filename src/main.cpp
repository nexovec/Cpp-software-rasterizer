#include "common_defines.hpp"
#include "asset_API.hpp"
#include "super_math.hpp"
#include "main.hpp"

Assets assets = Assets();

struct ColoredTrianglesVertexBuffer
{
    Vec_2f positions;
    Vec_4ui colors;
};

struct Triangle2D
{
    Vec_2f v1;
    Vec_2f v2;
    Vec_2f v3;
};

struct Quad2D
{
    Triangle2D bottom;
    Triangle2D bottom_uv;
    Triangle2D top;
    Triangle2D top_uv;
};

internal void clearScreen(ARGBTexture back_buffer)
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

    //  ? TODO: for performance reasons, you should decompose colors per-triangle, not per-pixel, but bitwise is fast and this won't be used anywway
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

internal void DEBUGrasterizeTriangle(ARGBTexture back_buffer, Triangle2D *triangle_ptr = 0)
{
    // SECTION: generate sample triangle
    Triangle2D triangle;
    Vec_2f x_vert = {200.0f, 250.0f};
    Vec_2f y_vert = {500.0f, 200.0f};
    Vec_2f z_vert = {350.0f, 350.0f};
    triangle = {x_vert, y_vert, z_vert};
    if (triangle_ptr)
        triangle = *triangle_ptr;

    // TODO: render wireframe
    // SECTION: rasterize triangles
    uint32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
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
                uint32 lower_x_bound = (uint32)(condition * x_bound + (!condition) * scanline_x_start[y]);
                uint32 higher_x_bound = (uint32)(condition * scanline_x_start[y] + !condition * x_bound);
                for (uint32 x = lower_x_bound; x < higher_x_bound; x++)
                {
                    Vec_2f vert_1 = triangle.v1;
                    Vec_2f vert_2 = triangle.v2;
                    Vec_2f vert_3 = triangle.v3;
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
internal inline uint32 DEBUGtextureColor(real32 lam_1, real32 lam_2, real32 lam_3, Vec_2f texel_1, Vec_2f texel_2, Vec_2f texel_3, ARGBTexture texture)
{
    // NOTE: uses brycentric coords
    // TODO: bilinear filtering
    // TODO: trilinear filtering
    int32 final_x = (int32)((texel_1.x * lam_1 + texel_2.x * lam_2 + texel_3.x * lam_3) + 0.5);
    int32 final_y = (int32)((texel_1.y * lam_1 + texel_2.y * lam_2 + texel_3.y * lam_3) + 0.5);

    return texture.bits[(final_y * texture.width) + final_x];
}
internal void rasterizeTriangleTextured(ARGBTexture back_buffer, Triangle2D *triangle_ptr, Triangle2D *uv_coords)
{
    // FIXME: code duplication
    Triangle2D triangle = *triangle_ptr;
    // TODO: render wireframe
    // SECTION: rasterize triangles
    real32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
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
                // PERFORMANCE: you can cache some things here and avoid excessive casts
                uint32 lower_x = (uint32)clamp<float>(min(x_bound, scanline_x_start[y]), 0, (real32)(back_buffer.width - 1));
                uint32 higher_x = (uint32)clamp<float>(max(x_bound, scanline_x_start[y]), 0, (real32)(back_buffer.width - 1));
                for (uint32 x = lower_x; x < higher_x; x++)
                {
                    Vec_2f vert_1 = triangle.v1;
                    Vec_2f vert_2 = triangle.v2;
                    Vec_2f vert_3 = triangle.v3;
                    // get barycentric coordinates
                    real32 det_t = (vert_2.y - vert_3.y) * (vert_1.x - vert_3.x) + (vert_3.x - vert_2.x) * (vert_1.y - vert_3.y);
                    real32 lam_1 = ((vert_2.y - vert_3.y) * (x - vert_3.x) + (vert_3.x - vert_2.x) * (y - vert_3.y)) / det_t;
                    real32 lam_2 = ((vert_3.y - vert_1.y) * (x - vert_3.x) + (vert_1.x - vert_3.x) * (y - vert_3.y)) / det_t;
                    real32 lam_3 = 1 - lam_2 - lam_1;
                    // PERFORMANCE: use gradient of the barycentric coordinates
                    // #if defined(DEBUG)
                    //                     back_buffer(x, y) = DEBUGtextureColor(lam_1, lam_2, lam_3, 0);
                    // #endif
                    back_buffer(x, y) = DEBUGtextureColor(lam_1, lam_2, lam_3, uv_coords->v1, uv_coords->v2, uv_coords->v3, assets.soldier.getUnderlyingTexture());
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
internal Quad2D generateAAQuad(Vec_2f pos, Vec_2f size)
{
    Triangle2D triangleA = {pos, pos + Vec_2f(1.0f, 0.0f) * size.y, pos + size};
    Triangle2D triangleA_uv = {{0.0f, 0.0f}, {512.0f, 0.0f}, {512.0f, 512.0f}};
    Triangle2D triangleB = {pos, pos + Vec_2f(0.0f, 1.0f) * size.x, pos + size};
    Triangle2D triangleB_uv = {{0.0f, 0.0f}, {0.0f, 512.0f}, {512.0f, 512.0f}};
    return {triangleA, triangleA_uv, triangleB, triangleB_uv};
}
internal void DEBUGrenderQuad2D(ARGBTexture back_buffer, Quad2D *quad)
{
    Triangle2D *as_array = (Triangle2D *)quad;
    for (int i = 0; i < 2; i++)
    {
        rasterizeTriangleTextured(back_buffer, as_array + i * 2, as_array + i * 2 + 1);
    }
}
void gameUpdateAndRender(ARGBTexture back_buffer)
{
    // NOTE: backbuffer format is ARGB
    // NOTE: backbuffer.width is default_scene_width; backbuffer_height is default_scene_height

    {
        // Render one triangle

        clearScreen(back_buffer);
        DEBUGrasterizeTriangle(back_buffer);
    }

    {
        // Render multiple triangles

        // Triangle2D triangle = {{0.0f, 0.0f}, {1280.f, 720.f}, {1280.f, 0.f}};
        // DEBUGrasterizeTriangle(back_buffer, &triangle);

        // TODO: create example
        constexpr Vec_2f midpoint = {640.f, 360.f};

        // FIXME: don't use persistent
        persistent Vec_2f rotating_point = {480.f, 280.f};
        rotating_point = Mat2x2f::rotationMatrix(0.1f) * (rotating_point - midpoint) + midpoint;
        Vec_2f other{0, 0};
        Vec_2f newly_generated;
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (real32)default_scene_width / 8 - 1, 0}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {default_scene_width - 1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {default_scene_width - 1, (i + 1.0f) * (real32)default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }

        other = {0, default_scene_height - 1};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {(i + 1.0f) * (real32)default_scene_width / 8, default_scene_height - 1}; //+((rand() % default_scene_width)-default_scene_width/2)
            Triangle2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }
        // FIXME: investigate why you can't use 0
        other = {1, 0};
        for (int i = 0; i < 8; i++)
        {
            newly_generated = {1, (i + 1.0f) * default_scene_height / 8 - 1}; //+((rand() % default_scene_width)-default_scene_width/2);
            Triangle2D triangle = {other, newly_generated, rotating_point};
            DEBUGrasterizeTriangle(back_buffer, &triangle);
            other = newly_generated;
        }
    }

    // TODO: print file info on file load
    // TODO: custom string classes
    // #ifdef DEBUG
    //     int32 h = (int32)assets.font_image.bh->bmp_info_header.Height;
    //     char buf[128];
    //     sprintf_s(buf, 128, "%ld\n", h);
    //     OutputDebugStringA(buf);
    // #endif

    // render image
    {

        BltBmp(&back_buffer, assets.soldier, 50, 150);
        BltBmp_fast(&back_buffer, assets.soldier, 700, 150);
        // font_tile_map.DEBUGdraw(&back_buffer, 6, 1, 200, 400);
    }

    // render text to screen
    {

        // TODO: set font alpha
        // TODO: font shadows
        // TODO: font outlines
        // TODO: remove whitespace before newline/NULL characters
        assets.font1.DEBUGrenderBitmapText(&back_buffer, (char *)"Don't be\nangry", 50, 100);
    }

    // draw texture-mapped quad
    {

        Quad2D quad = generateAAQuad(Vec_2f(900.0, 30.0), Vec_2f(400.0, 400.0));
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