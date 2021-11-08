#pragma once
#include "common_defines.hpp"
#include "platform_layer.hpp"
#pragma pack(push, 1)
struct BmpFileHeaderSection
{
    uint_16 FileType;     /* File type, always 4D42h ("BM") */
    uint_32 FileSize;     /* Size of the file in bytes */
    uint_16 Reserved1;    /* Always 0 */
    uint_16 Reserved2;    /* Always 0 */
    uint_32 BitmapOffset; /* Starting position of image data in bytes */
};
struct BmpInfoHeaderSection
{
    uint_32 Size;         /* Size of this header in bytes */
    int_32 Width;         /* Image width in pixels */
    int_32 Height;        /* Image height in pixels */
    uint_16 Planes;       /* Number of color planes */
    uint_16 BitsPerPixel; /* Number of bits per pixel */
};
struct BmpCompressionHeaderSection
{
    uint_32 Compression;     /* Compression methods used */
    uint_32 SizeOfBitmap;    /* Size of bitmap in bytes */
    int_32 HorzResolution;   /* Horizontal resolution in pixels per meter */
    int_32 VertResolution;   /* Vertical resolution in pixels per meter */
    uint_32 ColorsUsed;      /* Number of colors in the image */
    uint_32 ColorsImportant; /* Minimum number of important colors */
};
struct BmpColorHeader
{
    uint_32 RedMask;    /* Mask identifying bits of red component */
    uint_32 GreenMask;  /* Mask identifying bits of green component */
    uint_32 BlueMask;   /* Mask identifying bits of blue component */
    uint_32 AlphaMask;  /* Mask identifying bits of alpha component */
    uint_32 CSType;     /* Color space type */
    int_32 RedX;        /* X coordinate of red endpoint */
    int_32 RedY;        /* Y coordinate of red endpoint */
    int_32 RedZ;        /* Z coordinate of red endpoint */
    int_32 GreenX;      /* X coordinate of green endpoint */
    int_32 GreenY;      /* Y coordinate of green endpoint */
    int_32 GreenZ;      /* Z coordinate of green endpoint */
    int_32 BlueX;       /* X coordinate of blue endpoint */
    int_32 BlueY;       /* Y coordinate of blue endpoint */
    int_32 BlueZ;       /* Z coordinate of blue endpoint */
    uint_32 GammaRed;   /* Gamma red coordinate scale value */
    uint_32 GammaGreen; /* Gamma green coordinate scale value */
    uint_32 GammaBlue;  /* Gamma blue coordinate scale value */
};
struct BitmapHeader
{
    BmpFileHeaderSection bmp_file_header;
    BmpInfoHeaderSection bmp_info_header;
    BmpCompressionHeaderSection bmp_compression_header;
    BmpColorHeader bmp_color_header;
};

#pragma pack(pop)

struct bitmap_image
{
    bitmap_image setOpaquenessTo(uint_32 desired_alpha);
    argb_texture getUnderlyingTexture();
    static int loadBmpFromFile(bitmap_image *bmp, char *filepath);
    BitmapHeader *bh;
    uint_32 *pixels;
};