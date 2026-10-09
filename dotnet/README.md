# LiteScript.Net

.NET wrapper of [LiteScript](https://github.com/LePtitDev/lite-script), a little script engine to embed in applications: an object-oriented language similar to Javascript, an assembly language, a standard library, and states that can be saved and loaded in the middle of an execution.

The package contains the native engine for Windows (x64, arm64), Linux (x64, arm64) and macOS (x64, arm64), and targets .NET Standard 2.0 and .NET 8.

```
dotnet add package LiteScript.Net
```

## Usage

```csharp
using LiteScript;

using var memory = new Memory();
using var state = new State(memory);

// A function of the application
state.DeclareVariable("print", memory.RegisterFunction("print", (s, args) =>
{
    Console.WriteLine(string.Join(", ", args.Select(Operations.ToText)));
    return null;    // undefined
}));
Standard.Install(state);

Script.Result result = Script.Execute(state, @"
    function fibo(x) { return x < 2 ? 1 : fibo(x - 1) + fibo(x - 2); }
    print(fibo(10));
    return Math.sqrt(16);
");
if (!result.Success)
    Console.WriteLine($"{result.Line + 1}:{result.Column + 1} {result.GetMessage()}");
Console.WriteLine(result.Value);     // 4

// Calling a function of the script
Variable fibo = state.GetVariable("fibo")!;
Variable value = state.Call(fibo, memory.CreateNumber(5));
long n = value.AsNumber().ToInteger();     // 8
```

The API follows the [C++ API](https://github.com/LePtitDev/lite-script/blob/master/docs/guide/integration.md), the names are the same except for a few adaptations:

| C++ | .NET |
|-----|------|
| `Memory`, `State`, `Script`, `Assembly`, `Standard` | Same names |
| `Variable` (empty variable) | `Variable` (`null`) |
| `Type` | `ScriptType` |
| `v.Is<Number>()`, `v.As<Number>()` | `v.Is(ScriptType.Number)`, `v.AsNumber()` (also `AsBoolean`, `AsString`, `AsArray`, `AsClass`, `AsClassObject`, `AsNamespace`, `AsCallback`, `AsCharacter`) |
| `Array::indexed`, `Array::named` | `ArrayValue.Indexed` (`IList<Variable?>`), `ArrayValue.Named` (`MemberList`) |
| `Copy`, `Clone`, `Operate`, `GetMember`, `IsTruthy`... | `Operations.Copy`, `Operations.Clone`... |
| `ScriptError` | `ScriptException` |
| `SerializationError` | `LiteScript.SerializationException` |
| `SaveInstructions`, `LoadInstructions` | `Serialization.SaveInstructions`, `Serialization.LoadInstructions` |
| `Object::ReleaseCachedBlocks` | `Memory.ReleaseCachedBlocks` |
| `state.Save(stream)`, `state.Load(stream)` | `state.Save()` / `state.Save(stream)`, `state.Load(bytes)` / `state.Load(stream)` |

### Application functions

```csharp
Variable Sum(State state, IReadOnlyList<Variable?> args)
{
    Number total = 0;
    foreach (Variable? v in args)
    {
        if (v != null && v.Is(ScriptType.Number))
            total += v.AsNumber();
    }
    return state.Memory.CreateNumber(total);
}

state.DeclareVariable("sum", memory.RegisterFunction("sum", Sum));
```

- The arguments are only valid during the call: use `Duplicate()` to keep one.
- An exception thrown by the function becomes a script error that the script can catch with `try ... catch`. Throw a `ScriptException` to raise an error with a value: `throw new ScriptException(state.Memory.CreateString("invalid"))`.
- `RegisterFunction` gives a name to the function, which makes it possible to save the states that use it. `CreateCallback` creates an unnamed function.

### Errors

An error that the script doesn't catch is thrown as a `ScriptException` by `Execute`, `ExecuteSingle`, `Call`, `Script.Execute`...:

```csharp
try
{
    state.Execute();
}
catch (ScriptException error)
{
    Console.WriteLine($"Error at line {error.Position.Line + 1}: {error.Message}");
    Variable? thrown = error.Value;
}
```

The exceptions thrown by the debug and error hooks (`SetDebugHook`, `SetErrorHook`) reach the caller unchanged.

### Lifetime of the objects

- `Memory`, `State` and `Variable` are `IDisposable`. Disposing a `Variable` releases its reference to the object immediately; a variable that isn't disposed is released after the .NET garbage collection (by the next operation of its memory).
- Disposing a `Memory` disposes its states. The variables that still exist remain valid but must not be used with a state.
- The functions of the application are kept alive by their memory: a function that references its memory (or a state) keeps it alive until it's disposed.
- A memory and its states must be used from one thread at a time.

## Building from the sources

The sources are in the `dotnet` folder of the repository: the C interface of the engine (`native`), the .NET library (`src`) and its tests (`tests`).

```
# Build the native library for the current platform (Meson, Ninja and a C++20 compiler)
dotnet/build-native.ps1       # Windows
dotnet/build-native.sh        # Linux and macOS

# Build and test the .NET library (.NET SDK 9.0.200 or newer)
dotnet test dotnet/LiteScript.Net.slnx

# Create the package (with the native libraries found in dotnet/artifacts/runtimes)
dotnet pack dotnet/src/LiteScript.Net -c Release -o dotnet/artifacts/packages
```

The native library is the shared library `litescript_net`, built by Meson with the option `-Ddotnet=true`. The scripts copy it in `dotnet/artifacts/runtimes/<rid>/native`.

The workflow `.github/workflows/dotnet.yml` builds and tests the native library on every platform, then creates the package. It publishes the package on NuGet for the tags `v*` (the version is the one of the tag, `v2.1.0` gives `2.1.0`) with the secret `NUGET_API_KEY`, or when it's started manually with the option "publish".
