litescript_sources = files(
  'assembly.cpp',
  'memory.cpp',
  'number.cpp',
  'object.cpp',
  'operations.cpp',
  'script.cpp',
  'serialization.cpp',
  'standard.cpp',
  'state.cpp',
  'string.cpp',
)

litescript_lib = static_library('litescript',
  litescript_sources,
  include_directories: litescript_inc,
  install: true,
)

litescript_dep = declare_dependency(
  include_directories: litescript_inc,
  link_with: litescript_lib,
)
