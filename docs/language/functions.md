# Functions

## Declaration

```javascript
function add(a, b) {
    return a + b;
}

// Anonymous function in a variable
var sub = function(a, b) {
    return a - b;
};

// Calling an anonymous function directly
var eight = (function(x) { return x * 2; })(4);
```

A function without `return` (or with `return;`) returns `undefined`.

## Arguments

```javascript
function f(a, b, c) {
    // ...
}

f();            // a, b and c are undefined
f(3, "hello");  // c is undefined
f(1, 2, 3, 4);  // the fourth argument is ignored
```

Arguments are copies: modifying an argument doesn't modify the caller's variable. Since arrays and instances are shared, a function can however modify their content (see [values and references](variables-and-types.md#values-and-references)).

The last argument can collect all the remaining arguments in an array:

```javascript
function f1(args...) {
    for (var i = 0; i < args.length; i++)
        print(args[i]);
}

function f2(a, b, c...) {
    print(a, b, c[0]);
}

f1(1, 2, 3);    // args is [1, 2, 3]
f2(1, 2, 3, 4); // c is [3, 4]
```

## Local variables and closures

Variables declared in a function (arguments included) are local to each call. A function created inside another function keeps access to its variables:

```javascript
function counter() {
    var n = 0;
    return function() {
        n++;
        return n;
    };
}

var c = counter();
c();    // 1
c();    // 2
```

## this

A function retrieved as a member of an array or an instance is bound to that object, which it designates with `this`:

```javascript
var arr = [3, 2, 4, 1] + {
    // Selection sort
    sort: function() {
        for (var i = 0; i < this.length - 1; i++) {
            var min = i;
            for (var j = i + 1; j < this.length; j++) {
                if (this[j] < this[min])
                    min = j;
            }
            var tmp = this[i];
            this[i] = this[min];
            this[min] = tmp;
        }
    }
};

arr.sort();     // arr contains [1, 2, 3, 4]
```

Outside this case, `this` is `undefined`.

## Recursion

Functions can call themselves. The number of nested calls is limited by the application (100,000 by default); beyond that an [error](errors.md) is thrown.

```javascript
function fibo(n) {
    return n < 2 ? n : fibo(n - 1) + fibo(n - 2);
}
```
