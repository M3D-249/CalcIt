project_include_dirs = include_directories('../include')

src = files('test.cpp')


test_data = [
  'test_expressions.txt',
  'test_expressions_postfix.txt',
]

foreach f : test_data
  configure_file(
    input: f,
    output: f,
    copy: true
)
endforeach

test_exe = executable('calcit_test', src,
  include_directories: project_include_dirs,
  link_with: calcit_lib,
  install: true
)
gen_exe = executable('exp_gen', files('generate_expressions.cpp'))

install_data(['test_expressions.txt', 'test_expressions_postfix.txt'], install_dir: get_option('datadir'))
test('calcit_test', test_exe)