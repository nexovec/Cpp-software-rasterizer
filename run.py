config = {
    'platform': 'windows',
    'main':'"./../src/win32_platform.cpp"',
    'compiler_flags': [
        '-INCREMENTAL:YES',
        '-FC',
        '-Zi',
        '-O2',
        '-Femain.exe'
    ],
    'links':[
        'user32.lib',
        'Gdi32.lib'
    ],
    # 'run_debugger': True
    'run_debugger': False
}
import os
commands_win32 = [
    '@echo on',
    'pushd %~dp0',
    'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64',
    'mkdir build',
    'pushd %~dp0\\build',
    'rm .\\main.exe',
    'cl '+' '.join(config['compiler_flags'])+' '+config['main']+' '+' '.join(config['links']),
    'devenv .\\main.exe' if config['run_debugger']==True else 'START .\\main.exe',
]
build_batch_file_name = ".temp_build.bat"
with open(build_batch_file_name,'w') as f:
    for command in commands_win32:
        f.write(command+'\n')
os.system(build_batch_file_name)
os.remove(build_batch_file_name)
