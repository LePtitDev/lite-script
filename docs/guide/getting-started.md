# Getting started

## Requirements

- A C++20 compiler: MSVC 2022, GCC 11, Clang 14 or newer
- [Meson](https://mesonbuild.com) and [Ninja](https://ninja-build.org): `python -m pip install meson ninja`

## Building the library

```bash
meson setup build
meson compile -C build
meson test -C build
```

On Windows, run these commands in the "Developer PowerShell for VS 2022" (or add `--vsenv` to `meson setup`).

`meson setup` options:

| Option | Effect |
|--------|--------|
| `--buildtype=release` | Optimized build (`debugoptimized` by default) |
| `-Db_lto=true` | Link-time optimization (about 7% faster) |
| `-Dtests=false` | Doesn't build the tests ([doctest](https://github.com/doctest/doctest) is downloaded automatically) |
| `-Dexamples=false` | Doesn't build the examples |
| `-Dbenchmarks=true` | Builds the performance measurement program (`benchmarks/`) |
| `-Db_sanitize=address` | Builds with AddressSanitizer |

## Using the library in a project

**With Meson**: put the repository in your project's `subprojects/` folder (or use a `.wrap` file), then:

```meson
litescript_dep = dependency('litescript', fallback: ['litescript', 'litescript_dep'])
executable('my_application', 'main.cpp', dependencies: litescript_dep)
```

**Without Meson**: add the files from `src/` to your project and the `include/` folder to the include paths, in C++20.

A single header is enough:

```cpp
#include <litescript/litescript.hpp>
```

## Running a first script

```cpp
#include <iostream>
#include <litescript/litescript.hpp>

using namespace LiteScript;

int main() {
    // The memory holds the objects, the state holds the script execution
    // (the memory must be destroyed after the state)
    Memory memory;
    State state(memory);

    // An application function available in the script
    state.DeclareVariable("print", memory.RegisterFunction("print",
        [](State& s, std::vector<Variable>& args) {
            for (const Variable& v : args)
                std::cout << ToText(v).ToUTF8() << ' ';
            std::cout << std::endl;
            return s.GetMemory().CreateUndefined();
        }));

    // Compilation and execution
    Script::Result result = Script::Execute(state, R"(
        function fibo(x) {
            return x < 2 ? 1 : fibo(x - 1) + fibo(x - 2);
        }
        print("fibo(20) =", fibo(20));
    )");

    // Syntax errors
    if (!result) {
        std::cout << "Error at line " << result.line + 1 << ", column " << result.column + 1
                  << ": " << result.GetMessage() << std::endl;
        return 1;
    }
    return 0;
}
```

The programs in the `examples/` folder are two interactive interpreters, one for the script language and one for the [assembly language](../reference/assembly.md):

```bash
./build/examples/script examples/script/fibonacci.txt
./build/examples/assembly examples/assembly/fibonacci.txt
```

Next: [Integration](integration.md).
