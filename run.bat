@echo off
pushd %~dp0
echo Y | rmdir .\build /s /q
call .\premake5.exe vs2022
call MSBuild.exe Cpp-software-rasterizer.sln -m -ds:True -t:Rebuild /p:Configuration="Release" /p:Platform="Win64" /p:OutDir="./build/"

@REM call vcvarsall.bat x64
@REM call cl.exe src/** \link \DEBUG

echo launching the program
call .\build\Sandbox.exe
popd