#pragma once
#include "common_defines.h"
#pragma pack(push, 1)
struct BmpFileHeader
{
    uint16 FileType;     /* File type, always 4D42h ("BM") */
    uint64 FileSize;     /* Size of the file in bytes */
    uint16 Reserved1;    /* Always 0 */
    uint16 Reserved2;    /* Always 0 */
    uint64 BitmapOffset; /* Starting position of image data in bytes */
};
struct BmpInfoHeader
{
    uint64 Size;         /* Size of this header in bytes */
    int64 Width;         /* Image width in pixels */
    int64 Height;        /* Image height in pixels */
    uint16 Planes;       /* Number of color planes */
    uint16 BitsPerPixel; /* Number of bits per pixel */
};
struct BmpCompressionHeader
{
    uint64 Compression;     /* Compression methods used */
    uint64 SizeOfBitmap;    /* Size of bitmap in bytes */
    int64 HorzResolution;   /* Horizontal resolution in pixels per meter */
    int64 VertResolution;   /* Vertical resolution in pixels per meter */
    uint64 ColorsUsed;      /* Number of colors in the image */
    uint64 ColorsImportant; /* Minimum number of important colors */
};
struct BitmapHeader
{
    BmpFileHeader bmp_file_header;
    BmpInfoHeader bmp_info_header;
    // BmpCompressionHeader bch;
};

struct BmpColorHeader
{
    // TODO: use
    uint64 RedMask;    /* Mask identifying bits of red component */
    uint64 GreenMask;  /* Mask identifying bits of green component */
    uint64 BlueMask;   /* Mask identifying bits of blue component */
    uint64 AlphaMask;  /* Mask identifying bits of alpha component */
    uint64 CSType;     /* Color space type */
    int64 RedX;        /* X coordinate of red endpoint */
    int64 RedY;        /* Y coordinate of red endpoint */
    int64 RedZ;        /* Z coordinate of red endpoint */
    int64 GreenX;      /* X coordinate of green endpoint */
    int64 GreenY;      /* Y coordinate of green endpoint */
    int64 GreenZ;      /* Z coordinate of green endpoint */
    int64 BlueX;       /* X coordinate of blue endpoint */
    int64 BlueY;       /* Y coordinate of blue endpoint */
    int64 BlueZ;       /* Z coordinate of blue endpoint */
    uint64 GammaRed;   /* Gamma red coordinate scale value */
    uint64 GammaGreen; /* Gamma green coordinate scale value */
    uint64 GammaBlue;  /* Gamma blue coordinate scale value */
};
#pragma pack(pop)

struct BitmapImage
{
    static BitmapImage loadBitmapFromFile(char *filepath);
    static BitmapImage setColorAsAlpha(BitmapImage bmp, uint32 alpha_color);
    BitmapHeader *bh;
    uint32 *pixels;
};