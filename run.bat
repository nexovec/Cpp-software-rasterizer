@echo on
pushd %~dp0
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
pushd %~dp0\build
rm .\main.exe
cl -INCREMENTAL:NO -FC -Zi -O2 "./../src/main.cpp" user32.lib Gdi32.lib
@REM cl -INCREMENTAL:NO -FC -Zi -Od "./../src/main.cpp" user32.lib Gdi32.lib
@REM devenv .\main.exe
.\main.exe
pause
popd