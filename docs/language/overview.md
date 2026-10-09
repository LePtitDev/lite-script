# Language overview

## A scripting engine first

LiteScript is a scripting engine meant to be embedded in applications. The language therefore contains no built-in functions: the application adds the functions its scripts need (displaying text, reading a file, controlling a character...). If you write scripts for an application, find out which functions it provides.

The application can also enable the [standard library](standard-library.md) (`Math`, conversions, array and string methods).

## A quick tour

```javascript
// Functions
function fibo(n) {
    return n < 2 ? n : fibo(n - 1) + fibo(n - 2);
}

// Classes
class Point {
    var x = 0, y = 0;

    function Point(x, y) {
        this.x = x;
        this.y = y;
    }

    // Overloading the + operator
    operator add(p) {
        return new Point(this.x + p.x, this.y + p.y);
    }
}

var p = new Point(1, 2) + new Point(3, 4);

// Errors
try {
    undefined.x;
}
catch (e) {
    print(e.message);
}
```

## Differences from JavaScript

The language is very close to JavaScript; here is what changes.

**The semicolon is mandatory** at the end of statements:

```javascript
var a = 1.2;   // "var a = 1.2" without a semicolon is an error
a += 15;
```

**No prototypes**: they are replaced by [classes](classes.md) close to those of Java or C#, with multiple inheritance and operator overloading.

**No arrow functions** (`(a, b) => a + b`), only anonymous functions (`function(a, b) { return a + b; }`).

**Numbers** are 64-bit integers or floats: `6 / 2` is the integer `3`, `7 / 2` is `3.5`.

**No implicit conversion of strings to numbers**: `1 + "2"` is `null` (the operation is impossible), whereas `"1" + 2` is `"12"`.

**`var` declares a variable in the current function** (or in the current namespace outside functions). There is no `let` or `const`.

**Namespaces** group variables: see [Namespaces](namespaces.md).

## Keywords

The following keywords can't be used as variable names:

`var`, `function`, `class`, `static`, `operator`, `new`, `this`, `return`, `if`, `else`, `for`, `while`, `do`, `switch`, `case`, `default`, `break`, `continue`, `try`, `catch`, `throw`, `namespace`, `true`, `false`, `null`, `undefined`.

## Comments

```javascript
// Single-line comment

/* Multi-line
   comment */
```
