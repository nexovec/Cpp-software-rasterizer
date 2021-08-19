import sys
from subprocess import Popen, PIPE
import os
config = {
    'platform': 'windows',
    'main': 'win32_platform.cpp',
    'compiler_flags': [
        '-INCREMENTAL:YES',
        '-FC',
        '-Zi',
        '-O2'
    ],
    'links': [
        'user32.lib',
        'Gdi32.lib'
    ],
    # 'run_debugger': True, # defaults to false
    'pause_after_build': True  # defaults to false
}
config['compiler_flags'].append('-Fe'+'.'.join(config['main'].split('.')[:-1]))

thisdir = os.path.abspath(os.path.dirname(__file__))
main_exe_path = thisdir+'\\build\\' + \
    '.'.join(config['main'].split('.')[:-1])+'.exe'
code_files = []
for root, dirs, files in os.walk(thisdir+'\\src'):
    for file in files:
        if file.endswith(".c") or file.endswith(".cpp"):
            code_files.append(os.path.join(root, file))
cl_args = ' '.join([' '.join(config['compiler_flags']),
                   ' '.join(code_files), ' '.join(config['links'])])
commands_win32 = [
    '@echo off',
    'echo Compiling '+str(len(code_files))+' files',
    'setlocal EnableDelayedExpansion',
    'set "startTime=%time: =0%"',
    'pushd %~dp0',
    'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64',
    'mkdir \Q build',
    'pushd %~dp0\\build',
    'rm '+main_exe_path if os.path.exists(main_exe_path) else '',
    'cl '+cl_args,
    'START devenv ' + \
    main_exe_path if config.get(
        'run_debugger') == True else 'START '+main_exe_path,
    'set "endTime=%time: =0%"',
    'rem Get elapsed time:',
    'set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!startTime:%time:~8,1%=%%100)*100+1!"',
    'set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100), elap-=(elap>>31)*24*60*60*100"',
    'rem Convert elapsed time to HH:MM:SS:CC format:',
    'set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"',
    'echo Elapsed:  %hh:~1%%time:~2,1%%mm:~1%%time:~2,1%%ss:~1%%time:~8,1%%cc:~1%',
    'pause' if config.get('pause_after_build') == True else '',
    'popd',
    'popd'
]
build_batch_file_name = ".build.bat"
with open(build_batch_file_name, 'w') as f:
    for command in commands_win32:
        f.write(command+'\n')
if config.get('keep_build_file') == False:
    os.remove(build_batch_file_name)
