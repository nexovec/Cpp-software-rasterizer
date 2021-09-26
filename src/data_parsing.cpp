#include "data_parsing.h"
#include "platform_layer.h"
int BitmapImage::loadBitmapFromFile(BitmapImage *bmp, char *filepath)
{
    bmp->bh = (BitmapHeader *)(file_contents::readWholeFile(filepath, sizeof(BitmapHeader)).data);
    if (bmp->bh == 0)
    {
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