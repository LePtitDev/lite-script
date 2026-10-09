# Errors

## Engine errors

The following operations throw an error:

| Operation | Message |
|-----------|---------|
| Calling a value that isn't a function: `x()` | `a value of type number isn't a function` |
| Reading a member of `undefined` or `null`: `x.y` | `can't read the member "y" of undefined` |
| Reading an element of `undefined` or `null`: `x[0]` | `can't read an element of null` |
| Instantiating a value that isn't a class: `new x()` | `a value of type array isn't a class` |
| Calling an instance without a `call` operator | `the instance doesn't overload the call operator` |
| Exceeding the maximum number of nested calls | `maximum call depth exceeded` |

Application functions can also throw errors.

Operations between incompatible types don't throw an error, they give `null` (`1 + "a"`).

The value of an engine error is an array containing the elements `message`, `line` and `column` (the position of the error in the script, starting at 1).

## Throwing an error

`throw` throws any value:

```javascript
throw "invalid value";
throw { message: "invalid value", code: 3 };
throw new MyError();
```

## Catching an error

```javascript
try {
    process(data);
}
catch (e) {
    print("Error: " + e.message);
}

// The variable name is optional
try {
    risky();
}
catch {
    print("failed");
}
```

An error thrown in a function propagates up the calls to the nearest `try` block. The `catch` block can itself throw an error, caught by an enclosing `try` block.

There is no `finally` block.

## Uncaught errors

An error that isn't caught by any `try` block stops the script and propagates to the application, which can display the error's message and position (see [Integration](../guide/integration.md#runtime-errors)).
