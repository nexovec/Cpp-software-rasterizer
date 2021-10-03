-- premake5.lua
workspace "Test"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64", "Xbox360" }

project "HelloWorld"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++latest"
   targetdir "bin/%{cfg.buildcfg}"

   files { "src/**.hpp", "src/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"