project('CalcIt', 'cpp',
    version: '1.0.0',
    license: 'MIT',
    default_options: [
        'cpp_std=c++23',
        'buildtype=debugoptimized',
        'werror=true',
        'b_ndebug=if-release',
        'b_lto=true',
    ]
)

cpp = meson.get_compiler('cpp')
add_project_arguments(
    cpp.get_supported_arguments([
        '-fvisibility=hidden',
    ]),
    language: 'cpp'
)

if host_machine.system() == 'linux'
    # linux
elif host_machine.system() == 'windows'
    #widnows
endif

build_examples = get_option('examples')
build_tests = get_option('tests')

subdir('src')

if build_examples
    subdir('examples')
endif

if build_tests
    subdir('tests')
endif


src = files('src/calcit.cpp', 'src/main.cpp')

# build main exe
executable('CalcIt',
    src,
    dependencies: calcit_dep,
    install: true
)

