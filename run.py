import os
from subprocess import Popen, PIPE
import sys
config = {
    'platform': 'windows',
    'main': '"./../src/win32_platform.cpp"',
    'compiler_flags': [
        '-INCREMENTAL:YES',
        '-FC',
        '-Zi',
        '-O2',
        '-Femain.exe'
    ],
    'links': [
        'user32.lib',
        'Gdi32.lib'
    ],
    # 'run_debugger': True
    'run_debugger': False,
    'keep_build_file': False
}

thisdir = os.path.abspath(os.path.dirname(__file__))
code_files = []
for root, dirs, files in os.walk(thisdir+'\\src'):
    for file in files:
        if file.endswith(".c") or file.endswith(".cpp") or file.endswith(".h"):
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
    'rm .\\main.exe',
    '@echo on',
    'cl '+cl_args,
    '@echo off',
    'devenv .\\main.exe' if config['run_debugger'] == True else 'START .\\main.exe',
]
build_batch_file_name = ".build.bat"
with open(build_batch_file_name, 'w') as f:
    for command in commands_win32:
        f.write(command+'\n')
proc = Popen(build_batch_file_name, shell=True, stdout=PIPE)
switch = False
for line in proc.stdout:
    if line.startswith(str.encode('Microsoft')):
        switch = True
    if not switch:
        continue
    sys.stdout.buffer.write(line)
    sys.stdout.buffer.flush()
proc.stdout.close()
proc.wait()
if config['keep_build_file'] == False:
    os.remove(build_batch_file_name)
