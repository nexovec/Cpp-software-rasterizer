@echo on
pushd %~dp0
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
mkdir build
pushd %~dp0\build
rm .\main.exe
cl -INCREMENTAL:YES -FC -Zi -O2 "./../src/win32_platform.cpp" -Femain.exe user32.lib Gdi32.lib
devenv .\main.exe
@REM .\main.exe
pause
popd