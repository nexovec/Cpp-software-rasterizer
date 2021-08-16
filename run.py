# FIXME: I have no idea why this doesn't work
from subprocess import check_call
import os
commands = [
    'cd '+os.path.abspath(os.path.dirname(__file__)),
    'wincall call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64',
    'mkdir build',
    'pushd '+os.path.abspath(os.path.dirname(__file__))+'\\build',
    'wincall cl -INCREMENTAL:YES -FC -Zi -O2 "'+os.path.abspath(os.path.dirname(__file__)).replace('\\','/')+'/src/win32_platform.cpp" -Femain.exe user32.lib Gdi32.lib',
    'wincall devenv '+os.path.abspath(os.path.dirname(__file__))+'\\main.exe'
]
for command in commands:
    if command.lstrip().startswith('wincall '):
        check_call(command.lstrip()['wincall '.__len__():].lstrip())
    else:
        os.system(command)