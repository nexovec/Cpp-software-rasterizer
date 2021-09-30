#include "data_parsing.hpp"
#include "platform_layer.hpp"
ARGBTexture BitmapImage::getUnderlyingTexture()
{
    return {this->pixels, (uint32)this->bh->bmp_info_header.Width, (uint32)this->bh->bmp_info_header.Height};
}
int BitmapImage::loadBmpFromFile(BitmapImage *bmp, char *filepath)
{
    // NOTE: every bmp exported by gimp is fully transparent by default??
    // SECURITY: check validity of dimensions (are used as uint32)
    // FIXME: nobody keeps track of the underlying file data
    bmp->bh = (BitmapHeader *)(file_contents::readWholeFile(filepath, sizeof(BitmapHeader)).data);
    if (bmp->bh == 0)
    {
        // DEBUG: did you copy assets contents into build folder?
        TerminateProcess(1);
    }
    // bmp.bh = (BitmapHeader *)(file_contents::readWholeFile(filepath).data);
    bmp->pixels = (uint32 *)((uint8 *)bmp->bh + bmp->bh->bmp_file_header.BitmapOffset);

    // if (bmp.bh->bmp_file_header.FileType!=0x4D42)
    uint8 *file_type = reinterpret_cast<uint8 *>(&bmp->bh->bmp_file_header.FileType);
    if (file_type[0] != 'B' || file_type[1] != 'M')
    {
        // invalid file type
        TerminateProcess(1);
    }
    if (bmp->bh->bmp_info_header.Size > sizeof(BmpFileHeaderSection) + sizeof(BmpInfoHeaderSection))
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
    if (bmp->bh->bmp_info_header.Size > sizeof(BmpFileHeaderSection) + sizeof(BmpInfoHeaderSection) + sizeof(BmpCompressionHeaderSection))
    {
        // color header exists
    }

    return 1;
}
BitmapImage BitmapImage::setOpaquenessTo(uint32 desired_alpha)
{
    uint32 w = (uint32)this->bh->bmp_info_header.Width;
    uint32 h = (uint32)this->bh->bmp_info_header.Height;
    for (uint32 x = 0; x < w; x++)
    {
        for (uint32 y = 0; y < h; y++)
        {
            this->pixels[w * y + x] = this->pixels[w * y + x] | desired_alpha;
        }
    }
    return *this;
}