#include "common_defines.hpp"
#include "demos_3D.hpp"
#include "super_math.hpp"
#include "settings.hpp"
// #include <memory>

struct triangle_3D
{
    vec4_f v1;
    vec4_f v2;
    vec4_f v3;
    void transform(const mat4_f &);
    void rasterize(argb_texture &back_buffer);
};

internal void DEBUGrasterize_triangle_3D(argb_texture &back_buffer, triangle_3D *triangle_ptr = 0)
{
    // SECTION: generate sample triangle
    triangle_3D triangle;
    vec4_f x_vert = {200.0f, 250.0f, 0.f, 1.f};
    vec4_f y_vert = {500.0f, 100.0f, 0.f, 1.f};
    vec4_f z_vert = {350.0f, 350.0f, 0.f, 1.f};
    triangle = {x_vert, y_vert, z_vert};
    if (triangle_ptr)
        triangle = *triangle_ptr;

    // TODO: render wireframe
    // SECTION: rasterize triangles

    uint_32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
    vec4_f *vertices = (vec4_f *)&triangle;
    for (int i1 = 0; i1 < 3; i1++)
    {
        int i2 = (i1 + 1) % 3;
        vec4_f v1 = vertices[i1];
        vec4_f v2 = vertices[i2];
        vec4_f lower_vertex = v1;
        vec4_f higher_vertex = v2;
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
                uint_32 lower_x_bound = (uint_32)(condition * x_bound + !condition * scanline_x_start[y]);
                uint_32 higher_x_bound = (uint_32)(condition * scanline_x_start[y] + !condition * x_bound);
                for (uint_32 x = lower_x_bound; x < higher_x_bound; x++)
                {
                    vec4_f vert_1 = triangle.v1;
                    vec4_f vert_2 = triangle.v2;
                    vec4_f vert_3 = triangle.v3;
                    // get barycentric coordinates
                    real_32 det_t = (vert_2.y - vert_3.y) * (vert_1.x - vert_3.x) + (vert_3.x - vert_2.x) * (vert_1.y - vert_3.y);
                    real_32 lam_1 = ((vert_2.y - vert_3.y) * (x - vert_3.x) + (vert_3.x - vert_2.x) * (y - vert_3.y)) / det_t;
                    real_32 lam_2 = ((vert_3.y - vert_1.y) * (x - vert_3.x) + (vert_1.x - vert_3.x) * (y - vert_3.y)) / det_t;
                    real_32 lam_3 = 1 - lam_2 - lam_1;
                    // PERFORMANCE: use gradient of the barycentric coordinates
                    // #if defined(DEBUG)
                    //                     back_buffer(x, y) = DEBUGtextureColor(lam_1, lam_2, lam_3, 0);
                    // #endif
#define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
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

void triangle_3D::rasterize(argb_texture &back_buffer)
{
    DEBUGrasterize_triangle_3D(back_buffer, this);
}

void triangle_3D::transform(const mat4_f &transform)
{
    v1 = transform * v1;
    v2 = transform * v2;
    v3 = transform * v3;
}

struct quad_3D
{
    triangle_3D top;
    triangle_3D bottom;
    quad_3D(triangle_3D &top, triangle_3D &bottom);
    quad_3D(real_32 side);
    void transform(const mat4_f &);
    void rotate(real_32 x_rot, real_32 y_rot, real_32 z_rot, vec4_f &revolution_point);
};

quad_3D::quad_3D(real_32 side)
{
    vec4_f size = {side, side, 0.f, 0.f};
    // PERFORMANCE: initialize pos correctly instead of matrix multiply, the following is broken:
    // vec4_f pos = vec4_f(0.f, 0.f, 0.f, 1.f) - size;
    vec4_f pos = vec4_f(0.f, 0.f, 0.f, 1.f);
    top = { pos, pos + vec4_f(1.0f, 0.0f, 0.f, 0.f) * size.y, pos + size };
    bottom = { pos, pos + vec4_f(0.0f, 1.0f, 0.f, 0.f) * size.x, pos + size };
    this->transform(mat4_f::translation_matrix(-size/2));
}

quad_3D::quad_3D(triangle_3D &top, triangle_3D &bottom) : top(top), bottom(bottom) {}

void quad_3D::transform(const mat4_f &transform)
{
    top.transform(transform);
    bottom.transform(transform);
}

void quad_3D::rotate(real_32 x_rot, real_32 y_rot, real_32 z_rot, vec4_f& rev_point)
{
    mat4_f rot = mat4_f::rotation_matrix(x_rot, y_rot, z_rot);
    mat4_f pre_translation = mat4_f::translation_matrix(-rev_point);
    mat4_f post_translation = mat4_f::translation_matrix(rev_point);
    this->transform(pre_translation);
    this->transform(rot);
    this->transform(post_translation);
}


struct mesh_3D
{
    std::vector<vec4_f> vertex_data;
    mesh_3D(uint_32 reserved);
    mesh_3D &transform(mat4_f &);
};

mesh_3D::mesh_3D(uint_32 reserved_size = 128)
{
    // TODO: test
    // TODO: add push_vertices method
    vertex_data = std::vector<vec4_f>();
    // TODO: use reserved size
}

mesh_3D &mesh_3D::transform(mat4_f &transform_matrix)
{
    // TODO: test this
    for (int i = 0; i < vertex_data.size(); i++)
    {
        this->vertex_data[i] = transform_matrix * this->vertex_data[i];
    }
    return *this;
}

// struct cube_3D
// {
//     quad_3D near;
//     quad_3D far;
//     quad_3D left;
//     quad_3D right;
//     quad_3D top;
//     quad_3D bottom;
//     cube_3D(vec4_f &position, real_32 side);
// };

// cube_3D::cube_3D(vec4_f &position, real_32 side)
// {
    
//     // TODO: implement
// }

internal void DEBUGrender_quad_3D(argb_texture back_buffer, quad_3D *quad)
{
    triangle_3D *as_array = (triangle_3D *)quad;
    for (int i = 0; i < 2; i++)
    {
        // rasterize_triangle_textured(back_buffer, as_array + i * 2, as_array + i * 2 + 1, assets.soldier);
        DEBUGrasterize_triangle_3D(back_buffer, &(as_array[i]));
    }
}

void demo_render_3D_quad(argb_texture &back_buffer)
{
    // create the 3D quad
    vec4_f pos = {500.f, 400.f, 0.f, 1.f};
    quad_3D quad = quad_3D(100.f);

    mat4_f translation = mat4_f::translation_matrix(pos);
    quad.transform(translation);

    // quad.rotate(0, 0, 1, quad.top.v1);
    // quad.rotate(1,0,0,quad.top.v1);

    // FIXME: no matter what arguments, the quad stays the same
    mat4_f transform = mat4_f::screen_ortho_projection_matrix(0.f, 1280.f, 0.f, 720.f, 1.f, -1.f);
    // mat4_f transform = mat4_f::translation_matrix({200.f,0.f,0.f}) * 2 - mat4_f::unit_matrix() * 1;
    // mat4_f transform = mat4_f::unit_matrix();

    quad.transform(transform);

    DEBUGrender_quad_3D(back_buffer, &quad);
    return;
}