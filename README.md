# Software rasterizer in C++ [WIP]

## How to build

You need to have python, msvc and visual studio installed.<br />
FIXME: You need to copy the contents of asset folder into build folder<br />
Run `python generate_build.py`.<br />
You might need to open `generate_build.py` and set the correct `vcvarsall.bat` location.<br />
You might need to place `devenv.exe` (the one that launches visual studio) into PATH for the debugger to open automatically.<br />

## Conventions

We exclusively use defines in `common_defines.hpp` whenever possible.
TODO:

# Notes

All bmp files work when exported from GIMP 2.10.4 without color data and in XRGB 32 bit format.

## Before releases

inspect `// TEMPORARY:` sections and `DEBUG` def blocks
