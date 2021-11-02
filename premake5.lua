-- premake5.lua
workspace "Test"
    configurations { "Debug", "Release" }
    platforms {"Win64"}

project "HelloWorld"
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
      optimize "On"