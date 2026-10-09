doctest_dep = dependency('doctest', fallback: ['doctest', 'doctest_dep'])

test_sources = files(
  'main.cpp',
  'test_assembly.cpp',
  'test_documentation.cpp',
  'test_errors.cpp',
  'test_leaks.cpp',
  'test_memory.cpp',
  'test_operations.cpp',
  'test_script.cpp',
  'test_serialization.cpp',
  'test_standard.cpp',
  'test_state.cpp',
  'test_values.cpp',
)

tests_exe = executable('litescript_tests',
  test_sources,
  dependencies: [litescript_dep, doctest_dep],
)

# One test per suite to get a detailed report from "meson test"
foreach suite : ['number', 'string', 'memory', 'operations', 'script', 'state', 'assembly', 'serialization', 'errors', 'standard', 'documentation', 'leaks']
  test(suite, tests_exe,
    args: ['--test-suite=' + suite],
    env: { 'LITESCRIPT_DOCS': meson.project_source_root() / 'docs' },
    timeout: 300,
  )
endforeach
