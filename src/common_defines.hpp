#pragma once
#define global static
#define internal static
#define persistent static

#include <vector>

typedef char int_8;
typedef short int_16;
typedef int int_32;
typedef long long int_64;
typedef bool bool_32;

typedef unsigned char uint_8;
typedef unsigned short uint_16;
typedef unsigned int uint_32;
typedef unsigned long long uint_64;


typedef float real_32;
typedef double real_64;

// #define max(a,b) (((a>b)*a)&((a<=b)*b))
// #define min(a,b) (((a<=b)*a)&((a>b)*b))

// TEMPORARY: debug only, to fix vscode highlighting
#ifndef DEBUG
#define DEBUG
#endif