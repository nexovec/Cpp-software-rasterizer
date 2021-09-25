#pragma once
#define global static
#define internal static
#define persistent static

typedef char int8;
typedef short int16;
typedef int int32;
typedef long int64;
typedef bool bool32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

// TEMPORARY: debug only, to fix vscode highlighting
#ifndef DEBUG
#define DEBUG
#endif

class uint32_2D_array_wrapper_unsafe
{
    // ? TODO: Do we even want this whole thing?
    struct uint32_wrapper
    {
        uint32 *number;
        uint32_wrapper(uint32 *num) : number(num) {}
        uint32 operator*(uint32 other)
        {
            uint32 num = *(this->number);
            return num * other;
        }
        uint32_wrapper *operator=(uint32 other)
        {
            uint32 *num = this->number;
            *num = other;
            return this;
        }
        uint32_wrapper *operator=(uint32_wrapper other)
        {
            uint32 *num = this->number;
            *num = *other.number;
            return this;
        }
        uint32 operator==(uint32 other)
        {
            uint32 *num = this->number;
            return other == *num;
        }
    };
    struct row
    {
        uint32 *bits;
        row(uint32 *bits) : bits(bits) {}
        uint32_wrapper operator[](int32 y)
        {
            return uint32_wrapper(&this->bits[y]);
        }
    };
    uint32 *bits;
    uint32 width;

public:
    uint32_2D_array_wrapper_unsafe(uint32 *bits, uint32 width)
    {
        this->bits = bits;
        this->width = width;
    }
    inline row operator[](int32 x)
    {
        return row(&(this->bits[this->width * x]));
    }
};
