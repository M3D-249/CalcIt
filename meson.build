project('CalcIt', 'cpp',
    version: '1.0.0',
    meson_version: '>= 1.1',
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

subdir('include')

calcit_lib = static_library('calcit_lib', 'src/calcit.cpp',
    include_directories: get_variable('my_project_incdir')
)

subdir('src')

# Examples and tests
build_examples = get_option('examples')
build_tests = get_option('tests')

if build_examples
    subdir('examples')
endif

if build_tests
    subdir('tests')
endif