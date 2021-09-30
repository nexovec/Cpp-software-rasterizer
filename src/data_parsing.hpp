#pragma once
#include "common_defines.hpp"
#include "platform_layer.hpp"
#pragma pack(push, 1)
struct BmpFileHeaderSection
{
    uint16 FileType;     /* File type, always 4D42h ("BM") */
    uint32 FileSize;     /* Size of the file in bytes */
    uint16 Reserved1;    /* Always 0 */
    uint16 Reserved2;    /* Always 0 */
    uint32 BitmapOffset; /* Starting position of image data in bytes */
};
struct BmpInfoHeaderSection
{
    uint32 Size;         /* Size of this header in bytes */
    int32 Width;         /* Image width in pixels */
    int32 Height;        /* Image height in pixels */
    uint16 Planes;       /* Number of color planes */
    uint16 BitsPerPixel; /* Number of bits per pixel */
};
struct BmpCompressionHeaderSection
{
    uint32 Compression;     /* Compression methods used */
    uint32 SizeOfBitmap;    /* Size of bitmap in bytes */
    int32 HorzResolution;   /* Horizontal resolution in pixels per meter */
    int32 VertResolution;   /* Vertical resolution in pixels per meter */
    uint32 ColorsUsed;      /* Number of colors in the image */
    uint32 ColorsImportant; /* Minimum number of important colors */
};
struct BmpColorHeader
{
    uint32 RedMask;    /* Mask identifying bits of red component */
    uint32 GreenMask;  /* Mask identifying bits of green component */
    uint32 BlueMask;   /* Mask identifying bits of blue component */
    uint32 AlphaMask;  /* Mask identifying bits of alpha component */
    uint32 CSType;     /* Color space type */
    int32 RedX;        /* X coordinate of red endpoint */
    int32 RedY;        /* Y coordinate of red endpoint */
    int32 RedZ;        /* Z coordinate of red endpoint */
    int32 GreenX;      /* X coordinate of green endpoint */
    int32 GreenY;      /* Y coordinate of green endpoint */
    int32 GreenZ;      /* Z coordinate of green endpoint */
    int32 BlueX;       /* X coordinate of blue endpoint */
    int32 BlueY;       /* Y coordinate of blue endpoint */
    int32 BlueZ;       /* Z coordinate of blue endpoint */
    uint32 GammaRed;   /* Gamma red coordinate scale value */
    uint32 GammaGreen; /* Gamma green coordinate scale value */
    uint32 GammaBlue;  /* Gamma blue coordinate scale value */
};
struct BitmapHeader
{
    BmpFileHeaderSection bmp_file_header;
    BmpInfoHeaderSection bmp_info_header;
    BmpCompressionHeaderSection bmp_compression_header;
    BmpColorHeader bmp_color_header;
};

#pragma pack(pop)

struct BitmapImage
{
    BitmapImage setOpaquenessTo(uint32 desired_alpha);
    ARGBTexture getUnderlyingTexture();
    static int loadBmpFromFile(BitmapImage *bmp, char *filepath);
    BitmapHeader *bh;
    uint32 *pixels;
};