#pragma once
#include "platform_layer.hpp"
#include "asset_API.hpp"
#include "super_math.hpp"
void demo_draw_rotating_triangle_background(argb_texture back_buffer, vec2_f midpoint = {640.f, 360.f});
void demo_draw_simple_triangle(argb_texture back_buffer);
void demo_draw_texture_mapped_quad(argb_texture back_buffer, assets &assets);
void demo_draw_gui_demo(argb_texture back_buffer, assets& asset_cache);