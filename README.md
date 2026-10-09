# LiteScript - A little script engine

[![CI](https://github.com/LePtitDev/lite-script/actions/workflows/ci.yml/badge.svg)](https://github.com/LePtitDev/lite-script/actions/workflows/ci.yml)

LiteScript is a little script engine designed for developpers who want to include a script language in their application. The script language is similar to Javascript with object-oriented, and an assembler-like language gives access to the instructions of the engine. It's developed in C++20.

The objective is to create an alternative of current script engines and integrating features that aren't present (like a state saving to stop execution and store it in a file, or an execution function instruction by instruction, etc...).

The documentation is in the [docs](docs/README.md) folder.

> [!WARNING]
> **Warning**: This project is an old personal project that has been updated and modernized with the help of Anthropic's Claude AI, and it is meant to remain experimental. My free time is limited, so I do not plan to actively maintain it. You are free to use it as you wish, knowing this.

## Features

* Javascript like object-oriented language (classes, inheritance, operator overloading, namespaces, closures, try/catch)
* Assembler script language
* Execution of the whole script or instruction by instruction
* Calls of script functions from the application and of application functions from the script
* Runtime errors with their position, catchable by the scripts or by the application
* Save and load of the complete state of a running script
* Automatic memory management: reference counting and a garbage collector for the cycles
* Optional standard library (Math, conversions, methods of arrays and strings)
* A playground program (`litescript-pg`) to execute script files with libraries for the console, files, JSON, HTTP(S) and processes
* A .NET wrapper published on NuGet: [LiteScript.Net](dotnet/README.md)
* A Visual Studio Code extension (`editors/vscode`) with a language server (diagnostics, completion, navigation, rename) and a debugger (breakpoints, steps, variables, evaluation), given by `litescript-pg --lsp` and `--dap`
* Unit tests run by the continuous integration on Windows (MSVC), Linux (GCC, Clang) and macOS, with AddressSanitizer and UndefinedBehaviorSanitizer

*Types :*

* **undefined**
* **null**
* **boolean**
* **number** (64 bits integer or double)
* **string** (and **character** but it's hidden)
* **callback** (script or application function)
* **array**
* **namespace**
* **class** (inherit, static elements, constructor and operator overloading)
* **class object** (object created by a **class**)

## Build

The project uses [Meson](https://mesonbuild.com) and needs a C++20 compiler (MSVC 2022, GCC 11, Clang 14 or newer).

```bash
python -m pip install meson ninja
meson setup build
meson compile -C build
meson test -C build
```

On Windows, run these commands in a "Developer Command Prompt for VS 2022" (or add `--vsenv` to `meson setup`).

The scripts of the `scripts` directory do the same in `builddir/<build type>` (on Windows the Visual Studio environment is activated automatically):

```bash
scripts/build.sh [release|debug|debugoptimized] [clean] [-- meson options]
scripts/test.sh
```

On Windows: `scripts\build.bat` and `scripts\test.bat`.

Options:

* `-Dtests=false` : don't build the unit tests ([doctest](https://github.com/doctest/doctest) is downloaded automatically)
* `-Dexamples=false` : don't build the examples
* `-Dbenchmarks=true` : build the benchmarks (`benchmarks/`)
* `-Dplayground=false` : don't build the playground (`playground/`)
* `-Dhttp=disabled` : build the playground without HTTP client (WinHTTP on Windows, libcurl on the other systems)
* `-Db_sanitize=address` : build with AddressSanitizer
* `-Db_lto=true` : link-time optimization (about 7% faster)

To use the library in another Meson project, add it as a subproject and use `dependency('litescript')`.

## Usage

```cpp
#include <iostream>
#include <litescript/litescript.hpp>

using namespace LiteScript;

int main() {
    // The memory must outlive the states
    Memory memory;
    State state(memory);

    // A function of the application available in the script
    // (registered with a name to allow the saving of the state)
    state.DeclareVariable("print", memory.RegisterFunction("print",
        [](State& s, std::vector<Variable>& args) {
            for (const Variable& v : args)
                std::cout << ToText(v).ToUTF8() << ' ';
            std::cout << std::endl;
            return s.GetMemory().CreateUndefined();
        }));

    // Compile and execute a script
    Script::Result result = Script::Execute(state, R"(
        function fibo(x) {
            return x < 2 ? 1 : fibo(x - 1) + fibo(x - 2);
        }
        print("fibo(20) =", fibo(20));
    )");
    if (!result)
        std::cout << "ERROR(" << result.line + 1 << "," << result.column + 1 << "): " << result.GetMessage() << std::endl;

    // Call a script function from the application
    Variable fibo = state.GetVariable("fibo");
    std::cout << state.Call(fibo, { memory.CreateNumber(10) }) << std::endl;
}
```

Other possibilities:

* `Script::Compile(code)` gives the instructions without executing them, then `state.AddInstructions(...)` and `state.ExecuteSingle()` execute them one by one
* `state.Save(stream)` and `state.Load(stream)` save and restore a running script (the functions of the application must be registered with the same names before the loading)
* `Assembly::Compile(code)` and `Assembly::Disassemble(instructions)` convert between the assembler language and the instructions
* `memory.CollectGarbage()` destroys the cycles of unreachable objects immediately (it's also executed automatically when the number of objects grows)
* `Standard::Install(state)` gives the standard library to the scripts
* the errors that the scripts don't catch are thrown as `LiteScript::ScriptError` by `Execute` and `Call`

See the [integration guide](docs/guide/integration.md) for the details. The engine is also available for .NET with the package [LiteScript.Net](dotnet/README.md).

## Playground

`litescript-pg` executes script files with the standard library and additional libraries (`Console`, `File`, `Directory`, `Path`, `JSON`, `Http`, `Process`, `Time`):

```bash
litescript-pg "myfolder/*.lsc"
litescript-pg script.lsc -- arguments
```

```javascript
var response = Http.get("https://api.github.com/repos/LePtitDev/lite-script");
var repo = JSON.parse(response.body);
File.write("repo.json", JSON.stringify({ name: repo.full_name, stars: repo.stargazers_count }, 2));
print(Directory.list(".").join(", "));
```

See the [playground guide](docs/guide/playground.md), and [Editors and debugging](docs/guide/editors.md) for the extension of Visual Studio Code.

## Changes from the version 1

The engine has been rewritten in modern C++: the memory is managed with counted references and `std::variant`, the C++ API has changed, several bugs of the language have been fixed and some behaviors have changed (arrays and instances are shared by assignment, reading a missing element doesn't create it, runtime errors). See the [migration guide](docs/reference/migrating-from-v1.md).

## Example of Javascript like script

```javascript
// Enter in the namespace named "Game"
namespace Game;

// Person inherits Alive and Human
class Person : Alive, Human {

    // Attributes
    var name;
    var age;
    var gender;

    // Constructor
    function Person(name, age, gender) {
        // Parent constructor
        this.Alive();

        this.name = name;
        this.age = age;
        this.gender = gender;
    }

    // Methods
    function Birthday() {
        this.age++;
    }

    function Say(args...) {
        Console.WriteLine(this.name + " say : ");
        Console.Write("> ");
        for (var i = 0; i < args.length; i++) {
            Console.Write(args[i] + " ");
        }
        Console.WriteLine();
    }

    // Operator overloading
    operator add(p) {
        return Person.Marry(this, p);
    }

    // Static method
    static function Marry(p1, p2) {
        return new Person(Random.Name, 0, Random.Gender);
    }
}

// Return to the original namespace
namespace global;

var Anne = new Game.Person("Anne", 21, Gender.FEMALE);
var Henri = new Game.Person("Henri", 22, Gender.MALE);

print(Anne + Henri);
```
