#include "demos_3D.hpp"
#include "super_math.hpp"
// #include <memory>

struct triangle_3D{
    vec4_f v1;
    vec4_f v2;
    vec4_f v3;
};

struct mesh_3D{
    mesh_3D(mesh_3D& copy);
    uint_32 vertex_count;
    // std::shared_ptr<vec4_f> vertex_data;
    vec4_f *vertex_data;
    mesh_3D operator*=(mat4_f);
    ~mesh_3D();
};

mesh_3D::mesh_3D(mesh_3D& copy){
    this->vertex_count = copy.vertex_count;
    // this->vertex_data = make_shared<vec4_f>(copy.vertex_data);
}

mesh_3D mesh_3D::operator*=(mat4_f transform_matrix){
    // TODO: test this
    for(int i = 0; i < vertex_count; i++)
    {
        this->vertex_data[i] = transform_matrix * this->vertex_data[i];
    }
    return *this;
}
void demo_render_3D_quad(argb_texture& back_buffer)
{
    // TODO: implement
    return;
}