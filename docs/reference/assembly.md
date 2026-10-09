# Assembly language

The script language is compiled into elementary instructions, executed by a stack machine. The assembly language writes these instructions directly: one instruction per line, empty lines do nothing.

```asm
value.number 2
value.number 3
operator.mul
define.variable six
```

Line numbers (jumps and functions) start at 1. In string arguments, `\n`, `\r`, `\t`, `\0` and `\\` are escapes.

`Assembly::Compile` converts assembly code into instructions, `Assembly::Disassemble` converts instructions (for example those of a compiled script) into assembly code.

## How the machine works

- **The stack** holds the values of the calculations in progress: `value.*` instructions push a value onto it, operators pop their operands and push their result.
- **Arguments being prepared**: `push.args` creates an argument list, `define.arg` puts values in it, `operator.call` and `value.object` use it.
- **Calls**: calling a script function remembers the return position, creates a namespace for its local variables and jumps to its first instruction. `return` goes back to the caller and pushes the return value onto the stack.
- **Try blocks**: `try.begin` records the position of the `catch` block, an error jumps there and pushes the error value onto the stack.

## Definitions

| Instruction | Stack | Description |
|-------------|-------|-------------|
| `define.variable <name>` | 1 popped | Declares a variable in the current namespace |
| `define.arg <index>` | 1 popped | Sets an argument of the list being prepared |
| `define.return` | 1 popped | Sets the function's return value |

## Values

| Instruction | Stack | Description |
|-------------|-------|-------------|
| `value.pop` | 1 popped | Removes the last value |
| `value.copy` | 1 pushed | Duplicates the last value (same object) |
| `value.assign` | 1 popped, 1 pushed | Replaces the last value with a copy (assignment semantics) |
| `value.undefined`, `value.null` | 1 pushed | |
| `value.boolean <true\|false>` | 1 pushed | |
| `value.number <number>` | 1 pushed | |
| `value.string <string>` | 1 pushed | The string is the rest of the line after the space |
| `value.array` | 1 pushed | Empty array |
| `value.class` | 1 pushed | Empty class |
| `value.object` | 1 popped, 1 pushed | Instantiates the popped class with the argument list being prepared (and calls its constructor) |
| `value.callback <line>` | 1 pushed | Function that starts at the given line |
| `value.arg <index>` | 1 pushed | Copy of an argument of the current function |
| `value.args <index>` | 1 pushed | Array of copies of the arguments from the index |
| `value.this` | 1 pushed | Value of `this` |
| `value.variable <name>` | 1 pushed | Variable (undefined if it doesn't exist) |

## Stacks and calls

| Instruction | Description |
|-------------|-------------|
| `push.nsp` | Creates a local namespace |
| `pop.nsp` | Removes the local namespace and pushes its return value onto the stack |
| `push.args` | Creates an argument list being prepared |
| `pop.args` | Removes the last argument list being prepared |
| `return` | Goes back to the caller of the current function |

## Operations

Binary operators pop two values (the first operand is the second-to-last value), unary operators pop one value, and both push their result.

| Instruction | Operation | Instruction | Operation |
|-------------|-----------|-------------|-----------|
| `operator.assign` | `a = b` | `operator.log-not` | `!a` |
| `operator.unary-plus` | `+a` | `operator.log-and` | `a && b` |
| `operator.unary-minus` | `-a` | `operator.log-or` | `a \|\| b` |
| `operator.pre-incr` | `++a` | `operator.bit-not` | `~a` |
| `operator.post-incr` | `a++` | `operator.bit-and` | `a & b` |
| `operator.pre-decr` | `--a` | `operator.bit-or` | `a \| b` |
| `operator.post-decr` | `a--` | `operator.bit-xor` | `a ^ b` |
| `operator.add` | `a + b` | `operator.lshift` | `a << b` |
| `operator.sub` | `a - b` | `operator.rshift` | `a >> b` |
| `operator.mul` | `a * b` | `operator.add-assign` | `a += b` |
| `operator.div` | `a / b` | `operator.sub-assign` | `a -= b` |
| `operator.mod` | `a % b` | `operator.mul-assign` | `a *= b` |
| `operator.equ` | `a == b` | `operator.div-assign` | `a /= b` |
| `operator.dif` | `a != b` | `operator.array` | `a[b]` |
| `operator.great` | `a > b` | `operator.member <name>` | `a.name` (1 popped) |
| `operator.less` | `a < b` | `operator.call` | Calls the popped value with the argument list being prepared |
| `operator.great-equ` | `a >= b` | | |
| `operator.less-equ` | `a <= b` | | |

## Jumps

| Instruction | Description |
|-------------|-------------|
| `jump-to <line>` | Jumps to the line |
| `jump-if <line>` | Pops a value and jumps if it is true |
| `jump-else <line>` | Pops a value and jumps if it is false |

## Arrays and classes

These instructions pop a value and place it in the array or class then at the top of the stack.

| Instruction | Description |
|-------------|-------------|
| `array.push-numeric <index>` | `array[index] = value` |
| `array.push-literal <name>` | `array.name = value` |
| `class.push-static <name>` | Adds a static member |
| `class.push-unstatic <name>` | Adds an instance member (a function is a method) |
| `class.push-operator <index>` | Overloads the operator with the given index (order of the [operator table](#operator-index)) |
| `class.inherit` | Inherits from the popped class |
| `class.constructor <name>` | Sets the constructor (doesn't pop a value) |

## Namespaces and errors

| Instruction | Description |
|-------------|-------------|
| `namespace <name[.name]*>` | Changes the current namespace (`global` to go back to the global namespace) |
| `try.begin <line>` | Starts a try block whose catch block is at the given line |
| `try.end` | Ends the try block |
| `throw` | Pops a value and throws it as an error |

## Operator index

Used by `class.push-operator`: 0 `assign`, 1 `unary_plus`, 2 `unary_minus`, 3 `pre_increment`, 4 `post_increment`, 5 `pre_decrement`, 6 `post_decrement`, 7 `add`, 8 `subtract`, 9 `multiply`, 10 `divide`, 11 `modulo`, 12 `equal`, 13 `not_equal`, 14 `greater`, 15 `lesser`, 16 `greater_equal`, 17 `lesser_equal`, 18 `not`, 19 `and`, 20 `or`, 21 `bitwise_not`, 22 `bitwise_and`, 23 `bitwise_or`, 24 `bitwise_xor`, 25 `left_shift`, 26 `right_shift`, 27 `add_assign`, 28 `subtract_assign`, 29 `multiply_assign`, 30 `divide_assign`, 31 `get`, 32 `call`.

## Example: the Fibonacci sequence

```asm
namespace Program
jump-to 29
value.arg 0
define.variable n
value.variable n
value.number 1
operator.less-equ
jump-else 12
value.number 1
define.return
return
value.variable Fibonacci
push.args
value.variable n
value.number 1
operator.sub
define.arg 0
operator.call
value.variable Fibonacci
push.args
value.variable n
value.number 2
operator.sub
define.arg 0
operator.call
operator.add
define.return
return
value.callback 3
define.variable Fibonacci
namespace global
value.variable Program
operator.member Fibonacci
push.args
value.number 20
define.arg 0
operator.call
```

The file `examples/assembly/fibonacci.txt` contains a version of this example.
