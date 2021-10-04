#pragma once
#define global static
#define internal static
#define persistent static

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef bool bool32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef float real32;
typedef double real64;

// #define max(a,b) (((a>b)*a)&((a<=b)*b))
// #define min(a,b) (((a<=b)*a)&((a>b)*b))

// TEMPORARY: debug only, to fix vscode highlighting
#ifndef DEBUG
#define DEBUG
#endif