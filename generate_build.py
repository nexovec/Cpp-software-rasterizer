# FIXME: you hve to run python install psutil
import psutil
import sys
from subprocess import Popen, PIPE
import os
config = {
    'path_to_varsall': 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat',
    'platform': 'windows',
    'name': 'win32_platform',
    'compiler_flags': [
        '-INCREMENTAL:YES',
        '-FC',
        # '-ZI',
        '-Od',
        # '-O2',
        '-Isrc',
        '-DDEBUG',
        # '-analyze',
        # '-Wall',
        "-std:c++20"
    ],
    'links': [
        'user32.lib',
        'Gdi32.lib',
        # 'Xinput.lib'
    ],
    "examples": [  # TODO: output into build/examples subfolder
        {
            'name': "triangle",
            'needed': [
                'win32_platform.cpp',
                'triangle.cpp'
            ]
        }
    ],
    'launch_debugger': True, # defaults to False
    # 'run_executable': True,  # defaults to False
    'pause_after_build': True,  # defaults to False
    # 'build_examples': True  # defaults to False
}
# TODO: ensure good defaults

thisdir = os.path.abspath(os.path.dirname(__file__))
main_exe_path = thisdir+'\\build\\' + config.get('name')+'.exe'
code_files = []


def recursive_file_search(root_folder_name, cbk):
    for root, dirs, files in os.walk(thisdir+'\\'+root_folder_name):
        for file in files:
            cbk(file, root)


def main_cbk(file, root): return code_files.append(os.path.join(root, file)) if (
    file.endswith(".c") or file.endswith(".cpp")) else ''


recursive_file_search("src", main_cbk)
cl_args = ' '.join([' '.join(config['compiler_flags']), '-Fe'+config.get('name')+'.exe',
                   ' '.join(code_files), ' '.join(config['links'])])

cl_args_examples = []
for example in config.get("examples"):
    code_files_example = []

    def example_cbk(file, root):
        if os.path.basename(file) in example.get("needed"):
            print(file)
            code_files_example.append(os.path.join(root, file))
    recursive_file_search("src", example_cbk)
    recursive_file_search("examples", example_cbk)
    cl_args_example = ' '.join([' '.join(config['compiler_flags']), '-Feexamples\\'+example.get('name')+'.exe',
                                ' '.join(code_files_example), ' '.join(config['links'])])
    cl_args_examples.append(cl_args_example)
# print([p for p in sorted([p.name() for p in psutil.process_iter()]) if "devenv" in p])
commands_win32 = [
    '@echo off',
    'call "'+config.get('path_to_varsall')+'" x64',
    'echo Compiling '+str(len(code_files))+' files',
    'setlocal EnableDelayedExpansion',
    'set "startTime=%time: =0%"',
    'pushd %~dp0',
    'mkdir build',
    'pushd %~dp0\\build',
    'rm '+main_exe_path,
    'cl '+cl_args,  # + '>' + thisdir+'\\cl_output.txt',
    ''.join(''.join(map(lambda str: 'cl '+str+'\n', cl_args_examples)
                    ).split("\n")[:-1]) if config.get('build_examples')else '',
    'START devenv ' +
    main_exe_path if config.get('launch_debugger') == True and "devenv.exe" not in (p.name() for p in psutil.process_iter()) else '',
    'START '+main_exe_path if config.get("run_executable") else '',
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
commands_win32 = [elem for elem in commands_win32 if elem != '']
build_batch_file_name = ".build.bat"
with open(build_batch_file_name, 'w') as f:
    for command in commands_win32:
        f.write(command+'\n')

os.system(thisdir+'/.build.bat')
if config.get('keep_build_file') == False:
    os.remove(build_batch_file_name)
