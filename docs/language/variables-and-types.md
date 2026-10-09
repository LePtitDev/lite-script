# Variables and types

## Declaring a variable

```javascript
// Without a value: the variable is null
var myVariable;
// With a value
var otherVariable = 5.2;
// Several variables
var first, second = "text", third;
```

A variable name contains Latin letters (upper and lower case), digits and the characters `$` and `_`, and doesn't start with a digit.

Declaring a variable that already exists in the same function again replaces it. A variable declared in a function hides a variable of the same name declared outside.

## Types

Variables are dynamically typed: their type depends on the value they contain.

| Type | Examples | Notes |
|------|----------|-------|
| undefined | `undefined` | Value of a variable or member that doesn't exist, or of a missing argument |
| null | `null` | Value of a variable declared without a value, result of an impossible operation |
| boolean | `true`, `false` | |
| number | `42`, `-6`, `2.5`, `.8e-3` | 64-bit integer or double-precision float |
| string | `"Hello"`, `'¡Hola!'` | UTF-8; the escapes `\n`, `\t`, `\r`, `\0`, `é`, `\u{1F600}` are accepted |
| function | `function(x) { return x; }` | See [Functions](functions.md) |
| array | `[1, 2, 3]`, `{ name: "Percy", age: 17 }` | Associative array: elements indexed by numbers and by names |
| class | `class { ... }` | See [Classes](classes.md) |
| instance | `new Point(1, 2)` | |
| namespace | `namespace Math;` | See [Namespaces](namespaces.md) |

## Values and references

**Numbers, booleans and strings** are copied by assignment:

```javascript
var a = 5;
var b = a;
b++;            // a is still 5

var s = "abc";
var t = s;
t[0] = "x";     // s is still "abc", t is "xbc"
```

**Arrays, instances, classes and namespaces** are shared by assignment, as in JavaScript: both variables designate the same object.

```javascript
var a = [1, 2];
var b = a;
b[2] = 3;       // a is also [1, 2, 3]
b = [7];        // b designates another array, a is unchanged
```

Function arguments follow the same rule: a function can modify the content of an array or instance it receives as an argument, but not the caller's variable.

## Arrays

An array contains elements indexed by numbers and named elements:

```javascript
var a = [10, 20, 30];
var b = { name: "Percy", "age": 17 };

a[0];           // 10
a.length;       // 3 (number of elements indexed by numbers)
b.name;         // "Percy"
b["age"];       // 17

a[5] = 60;      // a[3] and a[4] are undefined, a.length is 6
b.city = "NY";  // adds a named element
```

Reading an element that doesn't exist gives `undefined` without creating the element: assignment is what creates it.

```javascript
var x = a[10];  // undefined, a.length doesn't change
```

Two arrays can be added: `[1, 2] + { x: 3 }` gives a new array containing the elements of both.

## Strings

```javascript
var s = "héllo";
s.length;       // 5 (number of characters)
s[1];           // "é"
s[0] = "H";     // s is "Héllo"
s + " !";       // "Héllo !"
"ab" * 3;       // "ababab"
```

## undefined and null

`undefined` indicates that a value doesn't exist: undeclared variable, missing member, argument not provided.

`null` is an empty value: the one of a variable declared without a value, or the result of an impossible operation (`1 + "a"`, `null * 2`...).

```javascript
function f(a, b) {
    if (b == undefined)
        return "only one argument";
}
```

The `|` operator gives the right-hand value when the left-hand one is `undefined` or `null`, which makes it possible to define default values:

```javascript
var name = parameters.name | "unknown";
```
