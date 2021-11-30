#include "demos_3D.hpp"
#include "super_math.hpp"
#include "settings.hpp"
// #include <memory>

struct triangle_3D{
    vec4_f v1;
    vec4_f v2;
    vec4_f v3;
    triangle_3D transform(mat4_f&);
    void rasterize(argb_texture& back_buffer);
};

internal void DEBUGrasterize_triangle_3D(argb_texture& back_buffer, triangle_3D* triangle_ptr = 0)
{
    // SECTION: generate sample triangle
    triangle_3D triangle;
    vec4_f x_vert = { 200.0f, 250.0f };
    vec4_f y_vert = { 500.0f, 100.0f };
    vec4_f z_vert = { 350.0f, 350.0f };
    triangle = { x_vert, y_vert, z_vert };
    if (triangle_ptr)
        triangle = *triangle_ptr;

    // TODO: render wireframe
    // SECTION: rasterize triangles

    uint_32 scanline_x_start[default_scene_width] = {}; // should probably be common for all triangles, should use unsigned short[]
    vec4_f* vertices = (vec4_f*)&triangle;
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

void triangle_3D::rasterize(argb_texture& back_buffer)
{
    // FIXME: remove:
    DEBUGrasterize_triangle_3D(back_buffer);

    // FIXME: broken??
    DEBUGrasterize_triangle_3D(back_buffer, this);
}

triangle_3D triangle_3D::transform(mat4_f& transform)
{
    v1 = transform * v1;
    v2 = transform * v2;
    v3 = transform * v3;
    return *this;
}

struct quad_3D {
    triangle_3D top;
    triangle_3D bottom;
    quad_3D transform(mat4_f&);
};

quad_3D quad_3D::transform(mat4_f& transform)
{
    top.transform(transform);
    bottom.transform(transform);
    return *this;
}

struct mesh_3D{
    mesh_3D(mesh_3D& copy);
    uint_32 vertex_count;
    // std::shared_ptr<vec4_f> vertex_data;
    vec4_f *vertex_data;
    mesh_3D& transform(mat4_f&);
    ~mesh_3D();
};

mesh_3D::mesh_3D(mesh_3D& copy){
    this->vertex_count = copy.vertex_count;
    // TODO: copy data as well
    this->vertex_data = copy.vertex_data;
    // this->vertex_data = make_shared<vec4_f>(copy.vertex_data);
}

mesh_3D& mesh_3D::transform(mat4_f& transform_matrix){
    // TODO: test this
    for(int i = 0; i < vertex_count; i++)
    {
        this->vertex_data[i] = transform_matrix * this->vertex_data[i];
    }
    return *this;
}

internal void DEBUGrender_quad_3D(argb_texture back_buffer, quad_3D* quad)
{
    triangle_3D* as_array = (triangle_3D*)quad;
    for (int i = 0; i < 2; i++)
    {
        // rasterize_triangle_textured(back_buffer, as_array + i * 2, as_array + i * 2 + 1, assets.soldier);
        // TODO: test
        DEBUGrasterize_triangle_3D(back_buffer, &(as_array[i]));
    }
}

void demo_render_3D_quad(argb_texture& back_buffer)
{
    // TODO: implement
    // create the 3D quad
    vec4_f pos = { 200., 300., 0., 1.0 };
    vec4_f size = {100., 100., 0.,1.};
    triangle_3D triangleA = { pos, pos + vec4_f(1.0f, 0.0f, 0.f, 1.f) * size.y, pos + size };
    triangle_3D triangleB = { pos, pos + vec4_f(0.0f, 1.0f, 0.f, 1.f) * size.x, pos + size };
    quad_3D quad = {triangleA, triangleB};

    // auto transform = mat4_f::ortho_projection_matrix(0, 800, 600, 0, -100, 500);
    // quad.transform(transform);

    triangleA.rasterize(back_buffer);
    //DEBUGrender_quad_3D(back_buffer, &quad);
    return;
}