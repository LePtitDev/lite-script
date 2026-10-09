# Operators

## List and precedence

A lower precedence number is applied first. The overload name is used to [overload the operator in a class](classes.md#overloaded-operators).

| Precedence | Operator | Symbol | Overload name |
|:----------:|----------|--------|---------------|
| 1 | Post-increment | `a++` | `post_increment` |
| 1 | Post-decrement | `a--` | `post_decrement` |
| 1 | Function call | `a(...)` | `call` |
| 1 | Element access | `a[b]` | `get` |
| 1 | Member access | `a.b` | |
| 2 | Pre-increment | `++a` | `pre_increment` |
| 2 | Pre-decrement | `--a` | `pre_decrement` |
| 2 | Copy | `+a` | `unary_plus` |
| 2 | Opposite | `-a` | `unary_minus` |
| 2 | Negation | `!a` | `not` |
| 2 | Bitwise negation | `~a` | `bitwise_not` |
| 2 | Instantiation | `new A(...)` | |
| 3 | Multiplication | `a * b` | `multiply` |
| 3 | Division | `a / b` | `divide` |
| 3 | Modulo | `a % b` | `modulo` |
| 4 | Addition | `a + b` | `add` |
| 4 | Subtraction | `a - b` | `subtract` |
| 5 | Left shift | `a << b` | `left_shift` |
| 5 | Right shift | `a >> b` | `right_shift` |
| 6 | Greater than | `a > b` | `greater` |
| 6 | Less than | `a < b` | `lesser` |
| 6 | Greater than or equal | `a >= b` | `greater_equal` |
| 6 | Less than or equal | `a <= b` | `lesser_equal` |
| 7 | Equality | `a == b` | `equal` |
| 7 | Inequality | `a != b` | `not_equal` |
| 8 | Bitwise AND | `a & b` | `bitwise_and` |
| 9 | Bitwise XOR | `a ^ b` | `bitwise_xor` |
| 10 | Bitwise OR | `a \| b` | `bitwise_or` |
| 11 | Logical AND | `a && b` | `and` |
| 12 | Logical OR | `a \|\| b` | `or` |
| 13 | Condition | `a ? b : c` | |
| 14 | Assignment | `a = b` | `assign` |
| 14 | Addition and assignment | `a += b` | `add_assign` |
| 14 | Subtraction and assignment | `a -= b` | `subtract_assign` |
| 14 | Multiplication and assignment | `a *= b` | `multiply_assign` |
| 14 | Division and assignment | `a /= b` | `divide_assign` |

Operators of the same precedence apply from left to right, except assignments and the condition, which apply from right to left (`a = b = 5` assigns 5 to `b` then to `a`).

## Operations by type

**Numbers**: operations between integers give an integer when the result is one (`6 / 2` is `3`, `7 / 2` is `3.5`). An integer that exceeds 64 bits becomes a float. Division by zero gives `Infinity` or `NaN`. Bitwise operations (`&`, `|`, `^`, `~`, `<<`, `>>`) use the integer part. A boolean is used as `0` or `1`.

**Strings**: `+` concatenates the string and the text of the other value (`"a" + 1` is `"a1"`), `*` repeats the string (`"ab" * 2` is `"abab"`), and comparisons follow character order.

**Arrays**: `+` gives a new array containing the elements of both arrays.

**Impossible operations**: an operation between incompatible types gives `null` (`1 + "2"`, `null * 3`, `-"a"`).

## Comparisons

`==` compares values without conversion: `1 == "1"` is false, `1 == 1.0` is true. Arrays, instances, classes and namespaces are equal if they are the same object. Two functions are equal if they run the same function.

`<`, `>`, `<=` and `>=` compare numbers with each other and strings with each other; other comparisons are false.

## Logical operators

`!`, `&&` and `||` give a boolean. The following are considered false: `undefined`, `null`, `false`, `0`, `NaN` and the empty string. All other values are true, including empty arrays.

Both operands of `&&` and `||` are always evaluated.

## Default value with |

When the left operand is not a number, `|` gives the right operand if the left one is `undefined` or `null`, and the left one otherwise:

```javascript
var name = options.name | "default";
```

## The condition

```javascript
var sign = x >= 0 ? "positive" : "negative";
```
