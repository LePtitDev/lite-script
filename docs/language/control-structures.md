# Control structures

Control structures are identical to those of JavaScript. Braces can be omitted when the block contains a single statement.

## if ... else

```javascript
if (condition) {
    // ...
}
else if (otherCondition) {
    // ...
}
else {
    // ...
}
```

The condition is false for `undefined`, `null`, `false`, `0`, `NaN` and the empty string.

## while

```javascript
while (condition) {
    // ...
}
```

## do ... while

```javascript
do {
    // ...
} while (condition);
```

## for

```javascript
for (var i = 0; i < 10; i++) {
    // ...
}

// Each part is optional, a missing condition is always true
for (;;) {
    // ...
}
```

## switch

```javascript
switch (value) {
    case 1:
    case 2:
        // ...
        break;
    case f(1):
    case 3 / 6 + 8 * f(2):
        // ...
        // Without break, execution continues into the next case
    default:
        // ...
}
```

Cases are arbitrary expressions, compared with `==`. The `default` case must be the last one.

## break and continue

`break` leaves the enclosing loop or `switch`, `continue` moves to the next iteration of the enclosing loop (including from within a `switch` or a `try` block).

## return

`return` leaves the current function. At the main level of a script, `return value;` stops the script's execution, and the application retrieves the value.

## try ... catch and throw

See [Errors](errors.md).
