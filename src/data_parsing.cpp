#include "data_parsing.h"
#include "platform_layer.h"
BitmapImage BitmapImage::setColorAsAlpha(BitmapImage bmp, uint32 alpha_color)
{
    // TODO:
    uint32_2D_array_wrapper_unsafe bmp_pixels = uint32_2D_array_wrapper_unsafe(bmp.pixels, bmp.bh->bmp_info_header.Width);

    for (int32 x = 0; x < bmp.bh->bmp_info_header.Width; x++)
    {
        for (int32 y = 0; y < bmp.bh->bmp_info_header.Height; y++)
        {
            // zero out if alpha is 0
            // bmp_pixels[y][x] = bmp_pixels[y][x] * ((*(bmp_pixels[y][x].number) >> 24) & 0xff != 0);
            // uint32 shouldZero = bmp_pixels[y][x] == alpha_color;
            // bmp_pixels[y][x] = bmp_pixels[y][x] * !shouldZero;
        }
    }
    return bmp;
}
BitmapImage BitmapImage::loadBitmapFromFile(char *filepath)
{
    // FIXME: error handling
    BitmapImage bmp;
    bmp.bh = (BitmapHeader *)(file_contents::readWholeFile(filepath).data);
    bmp.pixels = (uint32 *)(bmp.bh + bmp.bh->bmp_file_header.BitmapOffset);
    return bmp;
}