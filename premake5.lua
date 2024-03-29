-- premake5.lua
workspace "Cpp-software-rasterizer"
    configurations { "Debug", "Release" }
    platforms {"Win64"}
    warnings "Extra"

project "Sandbox"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"

   files { "src/**.hpp", "src/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      symbols "On"
      optimize "On"

include "examples"