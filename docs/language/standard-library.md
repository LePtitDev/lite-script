# Standard library

The standard library is **optional**: it is only available if the application has enabled it (see [Integration](../guide/integration.md#the-standard-library)).

## Math

| Function | Description |
|----------|-------------|
| `Math.abs(x)`, `Math.sign(x)` | Absolute value, sign (-1, 0 or 1) |
| `Math.floor(x)`, `Math.ceil(x)`, `Math.round(x)`, `Math.trunc(x)` | Rounding |
| `Math.sqrt(x)`, `Math.pow(x, y)`, `Math.exp(x)`, `Math.log(x)` | Square root, power, exponential, natural logarithm |
| `Math.sin(x)`, `Math.cos(x)`, `Math.tan(x)`, `Math.asin(x)`, `Math.acos(x)`, `Math.atan(x)`, `Math.atan2(y, x)` | Trigonometry (in radians) |
| `Math.min(a, b, ...)`, `Math.max(a, b, ...)` | Minimum and maximum |
| `Math.random()` | Random number in [0, 1[ |
| `Math.random(n)` | Random integer in [0, n[ |
| `Math.random(a, b)` | Random integer in [a, b] |
| `Math.seed(n)` | Seeds the random generator (to reproduce draws) |
| `Math.PI`, `Math.E` | Constants |

Functions give `null` if an argument isn't a number.

## Conversions

| Function | Description |
|----------|-------------|
| `typeOf(x)` | Type name: `"undefined"`, `"null"`, `"boolean"`, `"number"`, `"string"`, `"callback"`, `"array"`, `"class"`, `"class object"`, `"namespace"` |
| `toString(x)` | Text of a value |
| `parseNumber(s)` | Number written in a string (`" 4.5 "` gives `4.5`), `null` if the string isn't a number |
| `parseInt(s, base)` | Integer written in a string in a base from 2 to 36 (10 by default) |
| `isNaN(x)` | True if `x` isn't a number or is `NaN` |
| `char(code)` | Character for a Unicode code (`char(233)` gives `"é"`) |

## Array methods

| Method | Description |
|--------|-------------|
| `a.push(x, ...)` | Appends elements at the end, returns the new size |
| `a.pop()` | Removes and returns the last element |
| `a.insert(i, x)` | Inserts an element at position `i` |
| `a.remove(i)` | Removes and returns the element at position `i` (negative: from the end) |
| `a.clear()` | Removes all indexed elements |
| `a.indexOf(x)`, `a.contains(x)` | Position of `x` (-1 if absent), presence of `x` |
| `a.join(separator)` | Text of the elements joined by a separator (`,` by default) |
| `a.slice(start, end)` | New array with part of the elements (negative positions from the end) |
| `a.reverse()` | Reverses the order of the elements |
| `a.sort(compare)` | Sorts the elements (stable sort); the optional compare function returns a negative number if its first argument should be placed before the second |
| `a.keys()` | Array of the names of the named elements |
| `a.map(f)`, `a.filter(f)`, `a.forEach(f)` | Call `f(element, position)` for each element |

```javascript
var grades = [12, 8, 15];
grades.push(10);
grades.sort();                                      // [8, 10, 12, 15]
var doubled = grades.map(function(n) { return n * 2; });
var good = grades.filter(function(n) { return n >= 10; });
```

A named element takes precedence over a method of the same name.

## String methods

These methods never modify strings, they return new strings.

| Method | Description |
|--------|-------------|
| `s.substring(start, end)` | Part of the string |
| `s.indexOf(t, from)` | Position of `t` (-1 if absent) |
| `s.contains(t)`, `s.startsWith(t)`, `s.endsWith(t)` | Search |
| `s.split(separator)` | Array of the parts (empty separator: one element per character) |
| `s.replace(a, b)` | Replaces all occurrences of `a` with `b` |
| `s.trim()` | Removes spaces at the start and end |
| `s.toUpperCase()`, `s.toLowerCase()` | Upper and lower case (Latin letters) |
| `s.charCode(i)` | Unicode code of the character at position `i` |
