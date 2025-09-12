incdir = get_variable('my_project_incdir')

src = files('main.cpp')

dev = get_option('dev')

if dev
    src = files('main_dev.cpp')
endif

executable(
    'CalcIt',
    src,
    include_directories: incdir,
    link_with: calcit_lib,
    install: true
)