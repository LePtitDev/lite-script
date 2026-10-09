playground_sources = files(
  'src/console.cpp',
  'src/filesystem.cpp',
  'src/http.cpp',
  'src/json.cpp',
  'src/main.cpp',
  'src/system.cpp',
  'src/tools/api.cpp',
  'src/tools/dap.cpp',
  'src/tools/jsonvalue.cpp',
  'src/tools/lsp.cpp',
  'src/tools/transport.cpp',
)

playground_args = ['-DPLAYGROUND_VERSION="' + meson.project_version() + '"']
playground_deps = [litescript_dep]

# HTTP client: WinHTTP (part of Windows) or libcurl
http_option = get_option('http')
http_backend = 'none'
if host_machine.system() == 'windows'
  winhttp = cpp.find_library('winhttp', required: http_option)
  if winhttp.found()
    playground_deps += winhttp
    playground_args += '-DPLAYGROUND_HTTP_WINHTTP'
    http_backend = 'WinHTTP'
  endif
else
  curl = dependency('libcurl', required: http_option)
  if curl.found()
    playground_deps += curl
    playground_args += '-DPLAYGROUND_HTTP_CURL'
    http_backend = 'libcurl'
  endif
endif

if host_machine.system() == 'windows'
  # CommandLineToArgvW
  playground_deps += cpp.find_library('shell32')
endif

summary('HTTP client', http_backend, section: 'Playground')

playground_exe = executable('litescript-pg',
  playground_sources,
  # The internal headers of the engine give the conversions of numbers
  include_directories: include_directories('../src'),
  cpp_args: playground_args,
  dependencies: playground_deps,
  install: true,
)

if get_option('tests')
  # Each script of the tests directory is a test (in an empty temporary directory)
  foreach name : ['console', 'filesystem', 'json', 'system', 'http']
    test('playground-' + name, playground_exe,
      args: [meson.current_source_dir() / 'tests' / name + '.lsc', '--', 'first', 'second'],
      suite: 'playground',
      timeout: 60,
    )
  endforeach
endif
