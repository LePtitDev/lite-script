# Integration

This page describes the C++ API (namespace `LiteScript`, header `<litescript/litescript.hpp>`). The headers in `include/litescript/` document each function in detail.

## Memory and state

- `Memory` holds the objects manipulated by scripts (numbers, strings, arrays...) and the functions registered by the application.
- `State` holds the execution of a script: the instructions, the current position, the stack, the calls in progress and the global variables.

Several states can share one memory. The memory must be destroyed after its states. A memory and its states must be used from one thread at a time.

## Running code

```cpp
// Compile, add and execute
Script::Result result = Script::Execute(state, code);
if (!result)
    std::cout << result.line + 1 << ":" << result.column + 1 << " " << result.GetMessage();
Variable value = result.value;     // the value returned by "return" at the main level

// Or in several steps
Script::Result compiled = Script::Compile(code);
state.AddInstructions(compiled.instructions);
state.Execute();

// Instruction by instruction
while (!state.Finished())
    state.ExecuteSingle();
```

Each call to `AddInstructions` adds an instruction list that runs after the previous ones. Declared variables and functions stay available from one list to the next, which makes it possible to run a script in several pieces (as in an interactive console).

`Assembly::Execute` and `Assembly::ExecuteAfter` do the same with the [assembly language](../reference/assembly.md). `ExecuteAfter` appends the instructions to the end of the last list so that line numbers carry on.

## Values

A `Variable` is a reference-counted handle to an object in memory: copying it shares the object, which is destroyed along with its last reference.

```cpp
// Creation
Variable n = memory.CreateNumber(42);           // Number(42), Number(2.5)
Variable s = memory.CreateString("text");       // UTF-8
Variable b = memory.CreateBoolean(true);
Variable u = memory.CreateUndefined();
Variable a = memory.CreateArray({ n, s });      // [42, "text"]
Variable o = memory.CreateArray({ { "name", s }, { "value", n } });
Variable f = memory.CreateCallback([](State& st, std::vector<Variable>& args) { /* ... */ });

// Reading
if (n.Is<Number>()) {
    Number number = n.As<Number>();
    std::int64_t i = number.ToInteger();
    double d = number.ToFloat();
}
if (s.Is<String>())
    std::string text = s.As<String>().ToUTF8();
if (a.Is<Array>()) {
    Array& array = a.As<Array>();
    Variable first = array.indexed[0];
    Variable named = array.named.Find("name");  // empty if absent
}
Type type = n.GetType();                         // Type::Number
std::string description = n.ToString();          // description ("\"text\"" for a string)
String text = ToText(s);                         // plain text ("text")
```

The C++ types of values are `Undefined`, `Null`, `bool`, `Number`, `String`, `Character`, `Callback`, `Array`, `Class`, `ClassObject` and `Namespace`.

Useful functions to manipulate values with the language's semantics:

| Function | Description |
|----------|-------------|
| `Copy(memory, v)` | Copies like an assignment (arrays and instances are shared) |
| `Clone(memory, v)` | New independent array or instance |
| `Operate(memory, op, a, b)` | Applies an operator (`Operator::Add`...) |
| `GetMember(memory, v, name)` | Equivalent of `v.name` |
| `IsTruthy(v)` | Value of a condition |
| `ValuesEqual(a, b)`, `CompareValues(a, b)` | `==` and `<` comparisons (without overloading) |
| `Convert(memory, v, type)` | Conversion to a boolean, a number or a string |

## Script variables

```cpp
state.DeclareVariable("config", memory.CreateArray({ { "debug", memory.CreateBoolean(true) } }));
Variable v = state.GetVariable("result");       // empty if the variable doesn't exist
state.UseNamespace("Game.Engine");              // equivalent of "namespace Game.Engine;"
```

## Application functions

An application function receives the state and the arguments, and returns a value (an empty variable is equivalent to `undefined`):

```cpp
Variable Sum(State& state, std::vector<Variable>& args) {
    Number total;
    for (const Variable& v : args) {
        if (v.Is<Number>())
            total += v.As<Number>();
    }
    return state.GetMemory().CreateNumber(total);
}

state.DeclareVariable("sum", memory.RegisterFunction("sum", Sum));
```

`RegisterFunction` registers the function under a name, which makes it possible to [save](#saving-and-loading) states that use it. `CreateCallback` creates an unnamed function (a state that contains it can't be saved).

Inside an application function, `state.GetThis()` gives the value of `this` (the object the function is a member of) and `state.GetArg(i)` gives the arguments.

A C++ exception thrown by an application function becomes a [script error](../language/errors.md), which the script can catch with `try ... catch`:

```cpp
throw std::invalid_argument("invalid argument");
```

## Calling a script function

```cpp
Variable fibo = state.GetVariable("fibo");
Variable result = state.Call(fibo, { memory.CreateNumber(10) });
```

`Call` also accepts an instance that overloads the call operator. An application function can call a script function it receives as an argument.

## Runtime errors

An error that the script doesn't catch is thrown by `Execute`, `ExecuteSingle` and `Call` as a `ScriptError` exception:

```cpp
try {
    state.Execute();
}
catch (const ScriptError& error) {
    SourcePosition position = error.GetPosition();      // line and column starting at 0
    std::cout << "Error at line " << position.line + 1 << ": " << error.what() << std::endl;
    Variable value = error.GetValue();                  // the value thrown by the script
}
```

After an uncaught error, the current instruction list is stopped and the state remains usable: variables are kept and the following lists can be executed.

`state.SetMaxCallDepth(n)` changes the maximum number of nested calls (100,000 by default).

## Type methods

The application can add methods to the values of a type, called with `value.method(...)`, with `this` designating the value:

```cpp
Variable methods = memory.CreateNamespace();
methods.As<Namespace>().variables.Set("double", memory.RegisterFunction("number.double",
    [](State& s, std::vector<Variable>&) {
        Variable self = s.GetThis();
        return s.GetMemory().CreateNumber(self.As<Number>() * Number(2));
    }));
memory.SetMethods(Type::Number, methods);
// In the script: (21).double() is 42
```

## The standard library

```cpp
Standard::Install(state);           // all modules
Standard::InstallMath(state);       // or one module: Math
Standard::InstallConversions(state);
Standard::InstallArrays(state);
Standard::InstallStrings(state);
```

Its content is described in [Standard library](../language/standard-library.md).

## Saving and loading

A state can be saved at any time, including in the middle of execution, then loaded to continue running:

```cpp
std::ofstream out("state.bin", std::ios::binary);
state.Save(out);

// Later, possibly in another run of the application
Memory memory;
State state(memory);
// Application functions must be registered with the same names before loading
memory.RegisterFunction("print", Print);
Standard::Install(state);
std::ifstream in("state.bin", std::ios::binary);
state.Load(in);                     // replaces the content of the state
state.Execute();                    // continues the execution
```

The save contains the instructions, the position, the stack and all objects reachable from the state. If it is made during a call to an application function, the loaded state resumes right after that call, with `undefined` as the result.

`Save` throws a `SerializationError` if the state contains an unregistered function. `Load` throws a `SerializationError` if the stream is invalid, without modifying the state.

Instructions alone can be saved in binary with `SaveInstructions` and `LoadInstructions`, or as text with `Assembly::Disassemble` and `Assembly::Compile`.

## Memory management

An object is destroyed as soon as it is no longer referenced. Objects that reference each other (an array that contains itself, a function and the namespace that contains it...) are destroyed by the garbage collector.

The garbage collector runs automatically while scripts execute when the number of objects grows. It never destroys an object referenced by an application `Variable`, so it can be called at any time:

```cpp
std::size_t destroyed = memory.CollectGarbage();
std::size_t objects = memory.Count();
memory.SetGarbageCollectorThreshold(10000);     // 0 disables automatic execution
```
