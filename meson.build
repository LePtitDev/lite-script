project('litescript', 'cpp',
  version: '2.0.0',
  license: 'MIT',
  meson_version: '>= 1.1.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'buildtype=debugoptimized',
  ],
)

cpp = meson.get_compiler('cpp')
if cpp.get_argument_syntax() == 'msvc'
  # Sources are encoded in UTF-8
  add_project_arguments('/utf-8', language: 'cpp')
endif

litescript_inc = include_directories('include')

subdir('src')

meson.override_dependency('litescript', litescript_dep)

install_subdir('include/litescript', install_dir: get_option('includedir'))

pkg = import('pkgconfig')
pkg.generate(litescript_lib,
  description: 'A little script engine to embed in applications',
)

if get_option('examples') and not meson.is_subproject()
  subdir('examples')
endif

if get_option('benchmarks') and not meson.is_subproject()
  subdir('benchmarks')
endif

if get_option('tests') and not meson.is_subproject()
  subdir('tests')
endif
