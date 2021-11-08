#pragma once
#include "common_defines.hpp"
#include "data_parsing.hpp"
#include "platform_layer.hpp"
struct tile_map
{
    // FIXME: have generic image class
    bitmap_image imageData;
    uint_32 tile_width;
    uint_32 tile_height;
    uint_32 tiles_per_width;
    uint_32 tiles_per_height;
    tile_map(bitmap_image imageData, uint_32 tile_width, uint_32 tile_height);
    void DEBUGdraw(argb_texture *back_buffer, int_32 x, int_32 y, int_32 x_offset, int_32 y_offset);
    void DEBUGrenderBitmapText(argb_texture *back_buffer, char *text, int_32 x_offset, int_32 y_offset);
};
struct assets
{
    assets();
    bitmap_image soldier;
    tile_map font1;
    // ? FIXME: optional memory free
};
void BltBmp(argb_texture *back_buffer, bitmap_image bmp, int_32 x_offset, int_32 y_offset);
void BltBmp_fast(argb_texture *back_buffer, bitmap_image bmp, int_32 x_offset, int_32 y_offset);