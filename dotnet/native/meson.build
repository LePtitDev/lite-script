# Shared library used by the .NET wrapper (LiteScript.Net)
# Enabled with: meson setup <builddir> -Ddotnet=true

dotnet_link_args = []
if host_machine.system() == 'linux'
  # The package must run on the systems that don't have the same C++ runtime
  # (and its symbols and the ones of the engine aren't exported)
  dotnet_link_args += ['-static-libstdc++', '-static-libgcc', '-Wl,--exclude-libs,ALL']
endif

litescript_net_lib = shared_library('litescript_net',
  'litescript_net.cpp',
  dependencies: litescript_dep,
  gnu_symbol_visibility: 'hidden',
  link_args: dotnet_link_args,
  install: true,
)
