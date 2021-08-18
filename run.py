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
    'run_debugger': True, # defaults to false
    'keep_build_file': False
}
config['compiler_flags'].append('-Fe'+'.'.join(config['main'].split('.')[:-1]))
import os
from subprocess import Popen, PIPE
# import io, typing
import sys

thisdir = os.path.abspath(os.path.dirname(__file__))
main_exe_path = thisdir+'\\build\\'+'.'.join(config['main'].split('.')[:-1])+'.exe'
code_files = []
for root, dirs, files in os.walk(thisdir+'\\src'):
    for file in files:
        if file.endswith(".c") or file.endswith(".cpp"):
            code_files.append(os.path.join(root, file))
cl_args = ' '.join([' '.join(config['compiler_flags']),
                   ' '.join(code_files), ' '.join(config['links'])])

print("Compiling "+str(len(code_files))+" files")
commands_win32 = [
    '@echo off',
    'pushd %~dp0',
    'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64',
    'mkdir build' if not os.path.isdir(thisdir+'\\build') else '',
    'pushd %~dp0\\build',
    'rm '+main_exe_path, # TODO: extract outside of batch
    '@echo on',
    'cl '+cl_args,
    '@echo off',
    'START devenv /command "Tools.ImmediateMode" '+main_exe_path if config.get('run_debugger') == True else 'START '+main_exe_path,
]
build_batch_file_name = ".build.bat"
with open(build_batch_file_name, 'w') as f:
    for command in commands_win32:
        f.write(command+'\n')
# switch = False
# f = typing.TextIO()
proc = Popen(build_batch_file_name, shell=True, stdout=PIPE)
# while proc.poll() is None:
#     line = proc.stdout.readline().decode()
#     my_lines.append(line)
#     if(line.startswith(config['main'])):
#         switch = True
#     #     continue
#     # if switch:
#     #     print(line)
#     # print(line)
for line in proc.stdout:
    # pass
    # text = 'Microsoft (R) C/C++ Optimizing Compiler Version'
    # if line.decode().lstrip().startswith(text):
    #     switch = True
    # if switch:
    # f.buffer.write(line)
    sys.stdout.buffer.write(line)
    sys.stdout.buffer.flush()
    # sys.stdout.buffer.write(line)
proc.stdout.close()
proc.wait()
if config['keep_build_file'] == False:
    os.remove(build_batch_file_name)
