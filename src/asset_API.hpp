#pragma once
#include "common_defines.hpp"
#include "data_parsing.hpp"
#include "platform_layer.hpp"
struct Assets
{
    Assets();
    BitmapImage font_image;
    // ? FIXME: optional memory free
};
struct TileMap
{
    // FIXME: have generic image class
    BitmapImage imageData;
    uint32 tile_width;
    uint32 tile_height;
    uint32 tiles_per_width;
    uint32 tiles_per_height;
    TileMap(BitmapImage imageData, uint32 tile_width, uint32 tile_height);
    void DEBUGdraw(ARGBTexture *back_buffer, int32 x, int32 y, int32 x_offset, int32 y_offset);
    void DEBUGrenderBitmapText(ARGBTexture *back_buffer, char *text, int32 x_offset, int32 y_offset);
};
void DEBUGBltBmp(ARGBTexture *back_buffer, BitmapImage bmp, int32 x_offset, int32 y_offset);