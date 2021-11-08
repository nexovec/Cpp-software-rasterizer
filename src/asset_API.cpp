#include "super_math.hpp"
#include "data_parsing.hpp"
#include "platform_layer.hpp"
#include "asset_API.hpp"

tile_map::tile_map(bitmap_image imageData, uint_32 tile_width, uint_32 tile_height)
{
    this->imageData = imageData;
    this->tile_width = tile_width;
    this->tile_height = tile_height;
    this->tiles_per_height = imageData.bh->bmp_info_header.Height / this->tile_height;
    this->tiles_per_width = imageData.bh->bmp_info_header.Width / this->tile_width;
}
uint_32 alphaBlendColors_fast(uint_32 color_to, uint_32 color_from)
{
    uint_32 alpha = color_to >> 28;
    // uint_32 alpha = 10;
    uint_32 rgb1 = ((0xF - alpha) * (color_to & 0xF0F0F0));
    uint_32 rgb2 = (alpha * (color_from & 0xF0F0F0));
    return (rgb1 + rgb2) >> 4;
}
uint_32 alphaBlendColors(uint_32 color_to, uint_32 color_from)
{
    uint_32 alpha = color_to >> 24;
    // uint_32 alpha = 10;
    uint_32 rb1 = ((0x100 - alpha) * (color_to & 0xFF00FF)) >> 8;
    uint_32 rb2 = (alpha * (color_from & 0xFF00FF)) >> 8;
    uint_32 g1 = ((0x100 - alpha) * (color_to & 0x00FF00)) >> 8;
    uint_32 g2 = (alpha * (color_from & 0x00FF00)) >> 8;
    return ((rb1 + rb2) & 0xFF00FF) | ((g1 + g2) & 0x00FF00);
}

void BltBmp_fast(argb_texture *back_buffer, bitmap_image bmp, int_32 x_offset, int_32 y_offset)
{
    // TODO: blit texture instead of bitmap_image directly
    argb_texture tex = bmp.getUnderlyingTexture();
    for (int_32 y = y_offset; y < y_offset + (int_32)tex.height; y++)
    {
        for (int_32 x = x_offset; x < clamp(x_offset + (int_32)tex.width, 0, (int_32)back_buffer->width - 1); x++)
        {
            // PERFORMANCE: oof.
            // if (y < 0 || y >= tex.height)
            //     continue;
            // if (x < 0 || x >= tex.width)
            //     continue;
            back_buffer->bits[back_buffer->width * y + x] = alphaBlendColors_fast(bmp.pixels[(y - y_offset) * tex.width + x - x_offset], back_buffer->bits[back_buffer->width * y + x]);
        }
    }
}
void BltBmp(argb_texture *back_buffer, bitmap_image bmp, int_32 x_offset, int_32 y_offset)
{
    // TODO: blit texture instead of bitmap_image directly
    argb_texture tex = bmp.getUnderlyingTexture();
    for (int_32 y = 0; y < (int_32)tex.height && y + y_offset < back_buffer->height; y++)
    {
        for (int_32 x = 0; x < (int_32)tex.width && x + x_offset < back_buffer->width; x++)
        {
            // PERFORMANCE: oof.
            // if (y + y_offset < 0 || y_offset + y >= tex.height)
            //     continue;
            // if (x + x_offset < 0 || x + x_offset >= tex.width)
            //     continue;
            back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset] = alphaBlendColors(bmp.pixels[y * tex.width + x], back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset]);
        }
    }
}
void tile_map::DEBUGdraw(argb_texture *back_buffer, int_32 x, int_32 y, int_32 x_offset, int_32 y_offset)
{
    // FIXME: access violation on negative offsets
    // FIXME: This is copying bmp headers
    // FIXME: duplicate of BltBmp
    bitmap_image bmp = this->imageData;
    for (int_32 xx = 0; xx < (int_32)this->tile_width && y + y_offset < back_buffer->height; xx++)
    {
        for (int_32 yy = 0; yy < (int_32)this->tile_height && x + x_offset < back_buffer->width; yy++)
        {
            // HACK: the indexing, LUL.
            uint_32 new_color = bmp.pixels[((this->tiles_per_height - 1 - y) * this->tile_height + yy) * bmp.bh->bmp_info_header.Width + x * this->tile_width + xx];
            back_buffer->bits[back_buffer->width * (yy + y_offset) + xx + x_offset] = alphaBlendColors_fast(new_color, back_buffer->bits[back_buffer->width * (yy + y_offset) + xx + x_offset]);
        }
    }
}
void tile_map::DEBUGrenderBitmapText(argb_texture *back_buffer, char *text, int_32 x_offset, int_32 y_offset)
{
    // NOTE: new-lines resets the cursor to the original position and moves it 1 row downwards.
    // NOTE: this works with ASCII only tilemapped bitmap fonts. Their resolution is supplied on init.
    // FIXME: can try to access unsupported characters
    // TODO: auto-allign options
    int_32 index = 0;
    int_32 cursor_pos_vert = 0;
    int_32 cursor_pos_hor = 0;
    while (text[index] != '\0')
    {
        int_32 character = (int_32)text[index];
        index++;
        if(character == '\n')
        {
            cursor_pos_hor = 0;
            cursor_pos_vert--;
            continue;
        }
        
        if(character < 0x20 || character > 126) // if it is a printable character in ascii
        {
            character = '?';
        }
        this->DEBUGdraw(back_buffer, character % this->tiles_per_width, character / this->tiles_per_width, x_offset + cursor_pos_hor * this->tile_width, y_offset + this->tile_height * cursor_pos_vert);
        cursor_pos_hor++;
    }
}
static tile_map loadFont1(){
    bitmap_image font_image;
    // TODO: error checking
    bitmap_image::loadBmpFromFile(&font_image, (char *)"assets/font.bmp");
    // int_8 *path = (int_8 *)"font.bmp";
    // this->font_image = bitmap_image::loadBmpFromFile(path);
    // FIXME: this needs to get called, otherwise it throws?!
    font_image.setOpaquenessTo(0x22000000);
    return tile_map(font_image, 512 / 32, 96 / 4);
}
assets::assets():font1(loadFont1())
{

    // path = (int_8 *)"font.bmp";
    // FIXME: no safeguard against read errors
    bitmap_image::loadBmpFromFile(&this->soldier, (char *)"assets/soldier.bmp");
    this->soldier.setOpaquenessTo(0x22000000);

    this->font1 = loadFont1();
}