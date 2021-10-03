#include "data_parsing.hpp"
#include "platform_layer.hpp"
#include "asset_API.hpp"

TileMap::TileMap(BitmapImage imageData, uint32 tile_width, uint32 tile_height)
{
    this->imageData = imageData;
    this->tile_width = tile_width;
    this->tile_height = tile_height;
    this->tiles_per_height = imageData.bh->bmp_info_header.Height / this->tile_height;
    this->tiles_per_width = imageData.bh->bmp_info_header.Width / this->tile_width;
}
uint32 alphaBlendColors_fast(uint32 color_to, uint32 color_from)
{
    uint32 alpha = color_to >> 28;
    // uint32 alpha = 10;
    uint32 rgb1 = ((0xF - alpha) * (color_to & 0xF0F0F0));
    uint32 rgb2 = (alpha * (color_from & 0xF0F0F0));
    return (rgb1 + rgb2) >> 4;
}
uint32 alphaBlendColors(uint32 color_to, uint32 color_from)
{
    uint32 alpha = color_to >> 24;
    // uint32 alpha = 10;
    uint32 rb1 = ((0x100 - alpha) * (color_to & 0xFF00FF)) >> 8;
    uint32 rb2 = (alpha * (color_from & 0xFF00FF)) >> 8;
    uint32 g1 = ((0x100 - alpha) * (color_to & 0x00FF00)) >> 8;
    uint32 g2 = (alpha * (color_from & 0x00FF00)) >> 8;
    return ((rb1 + rb2) & 0xFF00FF) | ((g1 + g2) & 0x00FF00);
}

void DEBUGBltBmp_fast(ARGBTexture *back_buffer, BitmapImage bmp, int32 x_offset, int32 y_offset)
{
    for (int32 y = 0; y < bmp.bh->bmp_info_header.Height; y++)
    {
        for (int32 x = 0; x < bmp.bh->bmp_info_header.Width; x++)
        {
            back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset] = alphaBlendColors_fast(bmp.pixels[y * bmp.bh->bmp_info_header.Width + x], back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset]);
        }
    }
}
void DEBUGBltBmp(ARGBTexture *back_buffer, BitmapImage bmp, int32 x_offset, int32 y_offset)
{
    for (int32 y = 0; y < bmp.bh->bmp_info_header.Height; y++)
    {
        for (int32 x = 0; x < bmp.bh->bmp_info_header.Width; x++)
        {
            back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset] = alphaBlendColors(bmp.pixels[y * bmp.bh->bmp_info_header.Width + x], back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset]);
        }
    }
}
void TileMap::DEBUGdraw(ARGBTexture *back_buffer, int32 x, int32 y, int32 x_offset, int32 y_offset)
{
    // FIXME: access violation on negative offsets
    // FIXME: This is copying bmp headers
    BitmapImage bmp = this->imageData;
    for (int32 xx = 0; xx < (int32)this->tile_width; xx++)
    {
        for (int32 yy = 0; yy < (int32)this->tile_height; yy++)
        {
            // HACK: the indexing, LUL.
            uint32 new_color = bmp.pixels[((this->tiles_per_height - 1 - y) * this->tile_height + yy) * bmp.bh->bmp_info_header.Width + x * this->tile_width + xx];
            back_buffer->bits[back_buffer->width * (yy + y_offset) + xx + x_offset] = alphaBlendColors_fast(new_color, back_buffer->bits[back_buffer->width * (yy + y_offset) + xx + x_offset]);
        }
    }
}
void TileMap::DEBUGrenderBitmapText(ARGBTexture *back_buffer, char *text, int32 x_offset, int32 y_offset)
{
    // NOTE: new-lines resets the cursor to the original position and moves it 1 row downwards.
    // NOTE: this works with ASCII only tilemapped bitmap fonts. Their resolution is supplied on init.
    // FIXME: can try to access unsupported characters
    // TODO: auto-allign options
    int32 index = 0;
    int32 cursor_pos_vert = 0;
    int32 cursor_pos_hor = 0;
    while (text[index] != '\0')
    {
        int32 character = (int32)text[index];
        switch (character)
        {
        case '\n':
            cursor_pos_hor = 0;
            cursor_pos_vert--;
            break;
        default:
            this->DEBUGdraw(back_buffer, character % this->tiles_per_width, character / this->tiles_per_width, x_offset + cursor_pos_hor * this->tile_width, y_offset + this->tile_height * cursor_pos_vert);
            cursor_pos_hor++;
            break;
        }
        index++;
    }
}
Assets::Assets()
{
    // int8 *path = (int8 *)"font.bmp";
    // this->font_image = BitmapImage::loadBmpFromFile(path);
    BitmapImage::loadBmpFromFile(&this->font_image, (char *)"assets/font.bmp");
    this->font_image.setOpaquenessTo(0x22000000);
    // FIXME: this needs to get called, otherwise it throws?!

    // path = (int8 *)"font.bmp";
    BitmapImage::loadBmpFromFile(&this->soldier, (char *)"assets/soldier.bmp");
    this->soldier.setOpaquenessTo(0x22000000);
    // FIXME: no safeguard against read errors
    return;
}