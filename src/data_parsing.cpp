#include "data_parsing.hpp"
#include "platform_layer.hpp"
argb_texture bitmap_image::get_underlying_texture()
{
    return {this->pixels, (uint_32)this->bh->bmp_info_header.Width, (uint_32)this->bh->bmp_info_header.Height};
}
int bitmap_image::load_bmp_from_file(bitmap_image *bmp, char *filepath)
{
    // NOTE: every bmp exported by gimp is fully transparent by default??
    // SECURITY: check validity of dimensions (are used as uint_32)
    // FIXME: nobody keeps track of the underlying file data
    bmp->bh = (bitmap_header *)(file_contents::readWholeFile(filepath, sizeof(bitmap_header)).data);
    if (bmp->bh == 0)
    {
        // DEBUG: did you copy assets contents into build folder?
        TerminateProcess(1);
    }
    // bmp.bh = (bitmap_header *)(file_contents::readWholeFile(filepath).data);
    bmp->pixels = (uint_32 *)((uint_8 *)bmp->bh + bmp->bh->bmp_file_header.BitmapOffset);

    // if (bmp.bh->bmp_file_header.FileType!=0x4D42)
    uint_8 *file_type = reinterpret_cast<uint_8 *>(&bmp->bh->bmp_file_header.FileType);
    if (file_type[0] != 'B' || file_type[1] != 'M')
    {
        // invalid file type
        TerminateProcess(1);
    }
    if (bmp->bh->bmp_info_header.Size > sizeof(bmp_file_header_section) + sizeof(bmp_info_header_section))
    {
        // compression header exists
        // TODO: convert to 32 bits per pixel
        switch (bmp->bh->bmp_info_header.BitsPerPixel)
        {
        case 32:
            break;
        case 24:
        {
        }
        break;
        default:
            TerminateProcess(1);
            break;
        }

        // TODO: decompress
        switch (bmp->bh->bmp_compression_header.Compression)
        {
        case 0:
            break;
        case 3:
            break;
        default:
            TerminateProcess(1);
            break;
        }
    }
    if (bmp->bh->bmp_info_header.Size > sizeof(bmp_file_header_section) + sizeof(bmp_info_header_section) + sizeof(bmp_compression_header_section))
    {
        // color header exists
    }

    return 1;
}
bitmap_image bitmap_image::set_opaqueness_to(uint_32 desired_alpha)
{
    uint_32 alpha_shifted = desired_alpha << 24;
    uint_32 w = (uint_32)this->bh->bmp_info_header.Width;
    uint_32 h = (uint_32)this->bh->bmp_info_header.Height;
    for (uint_32 x = 0; x < w; x++)
    {
        for (uint_32 y = 0; y < h; y++)
        {
            this->pixels[w * y + x] = this->pixels[w * y + x] | alpha_shifted;
        }
    }
    return *this;
}