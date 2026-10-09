# Namespaces

A namespace groups variables. The `namespace` statement changes the namespace in which the following variables are declared:

```javascript
// Enters the "Math" namespace (created if it doesn't exist)
namespace Math;

// "add" is declared in "Math"
function add(a, b) {
    return a + b;
}

// Namespaces can be nested
namespace Game.Engine;
var version = 2;

// Back to the global namespace (the default one)
namespace global;

var a = Math.add(3, 2);     // 5
var v = Game.Engine.version; // 2
```

Variables are looked up in the current function, then in the current namespace, then in the global namespace. Inside the `Math` namespace, `add(1, 1)` is enough.

A variable can be added to a namespace from outside: `Math.zero = 0;`.

A namespace can't be replaced by an assignment (`Math = 5;` has no effect).
