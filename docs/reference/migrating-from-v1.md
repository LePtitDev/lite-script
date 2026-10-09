# Migrating from version 1

Version 2 is a rewrite of the engine in C++20. The script language and the assembly language are kept, with a few behavior changes. The C++ API has changed.

## Building

CMake is replaced by Meson (see [Getting started](../guide/getting-started.md)). Headers are in `include/litescript/` and are included with `#include <litescript/litescript.hpp>`.

## The C++ API

| Version 1 | Version 2 |
|-----------|-----------|
| `CreateVariable(memory, 5)`, `CreateVariable(memory, "a")`... | `memory.CreateNumber(5)`, `memory.CreateString("a")`... |
| `CreateUndefined(memory)` | `memory.CreateUndefined()` |
| `CreateVariable(memory, count, values)` | `memory.CreateArray({ ... })` |
| `CreateVariable(memory, function)` | `memory.RegisterFunction(name, function)` or `memory.CreateCallback(function)` |
| `AddCallback` / `CreateCallback(memory, id)` / `Callback::List` | `memory.RegisterFunction(name, function)`: functions are found again by name when loading |
| `DeclareVariable(state, name, v)` | `state.DeclareVariable(name, v)` |
| `Script script(state); script.Execute(code); script.error` | `Script::Result r = Script::Execute(state, code); if (!r) ...` |
| `Script::GetInstructionList(code, error, line, col)` | `Script::Compile(code)` (the result contains `instructions`, `error`, `line`, `column`) |
| `Assembly assembly(state); assembly.ExecuteAfter(code)` | `Assembly::ExecuteAfter(state, code)` |
| `Instruction::Save` / `Instruction::Load` (text) | `Assembly::Disassemble` / `Assembly::Compile` |
| `Instruction::SaveBinary` / `LoadBinary` | `SaveInstructions` / `LoadInstructions` |
| `Memory::Save` / `Memory::Load` + `State::Save` / `State::Load` | `state.Save(stream)` / `state.Load(stream)` (the memory used by the state is included) |
| `memory.GarbageCollector(state)` | `memory.CollectGarbage()` (also run automatically) |
| `v->GetData<Number>()` | `v.As<Number>()` |
| `v->GetType() == Type::NUMBER` | `v.Is<Number>()` or `v.GetType() == Type::Number` |
| `(std::string)v` | `v.ToString()` or `ToText(v).ToUTF8()` |
| `Nullable<Variable>` | Empty `Variable` (`if (v)`) |
| `InstrCode::INSTR_VALUE_NUMBER` | `InstrCode::ValueNumber` |
| Application function `Variable f(State&, std::vector<Variable>&)` | Unchanged (any function or lambda with this signature) |

**Custom types** (classes derived from `Type`) are no longer possible: values are a fixed set of types. Application functions, script classes and [type methods](../guide/integration.md#type-methods) replace them.

**`Variable` is a reference-counted handle**: `v = other` shares the object instead of applying the script's assignment. For the script's assignment, use `Operate(memory, Operator::Assign, v, other)`.

## Language changes

**Fixes**: `this` is available in methods, the `? :` operator and the `not_equal` overload work, `args...` gives the arguments from index 0, keywords are recognized as whole words (`iffy` is no longer `if`), Windows line endings are accepted, a local variable can hide a global variable, `break` and `continue` work in all loops, `!!a` and `a.b++` are accepted.

**Behavior changes**:

- Arrays, instances, classes and namespaces are **shared** by assignment (as in JavaScript) instead of being copied.
- Reading an element that doesn't exist (`a[5]`, `a.x`) gives `undefined` **without creating the element**; assignment is what creates it.
- `undefined` stays `undefined` when assigned (it used to become `null`). `var x;` is still `null`.
- Function arguments are copies (modifying an argument no longer modifies the caller's variable).
- `7 / 2` is `3.5` (integer division only applies if the result is an integer).
- Numbers are 64-bit integers or double-precision floats (instead of 32 bits).
- Logical operators consider `undefined`, `null`, `false`, `0`, `NaN` and the empty string to be false.
- Calling a value that isn't a function, reading a member of `undefined` or `null`, instantiating a value that isn't a class and exceeding the call depth **throw an error** instead of giving `undefined`.

**New features**: `try ... catch`, `throw`, the optional [standard library](../language/standard-library.md), array `length`, `\u` escapes in strings.

## The assembly language

Instructions are unchanged, with a few clarifications:

- Empty lines are accepted (they do nothing).
- `\n`, `\r`, `\t`, `\0` and `\\` are escapes in string arguments.
- `value.arg` gives a copy of the argument.
- `push.nsp` creates a local namespace.
- `pop.args` doesn't remove the argument lists prepared by the function's caller.
- New instructions: `try.begin`, `try.end` and `throw`.
