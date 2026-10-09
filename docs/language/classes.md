# Classes

## Declaration

```javascript
class Person {

    // Attributes (with or without a default value)
    var name, age = 0;

    // Constructor: the method named like the class
    function Person(name, age) {
        this.name = name;
        this.age = age;
    }

    // Method
    function birthday() {
        this.age++;
    }

}

var anne = new Person("Anne", 21);
anne.birthday();
anne.age;       // 22
```

There is no notion of public, protected or private members: all members are accessible.

Each instance receives a copy of its attributes' default values: an attribute `var list = [];` gives a different array to each instance.

A member can be added to an instance by assignment: `anne.city = "Paris";`.

## Anonymous classes

```javascript
// The constructor of an anonymous class is called "constructor"
var Point = class {
    var x, y;
    function constructor(x, y) {
        this.x = x;
        this.y = y;
    }
};

// Direct instantiation
var instance = new (class {
    var a = 1;
})();
```

## Inheritance

A class can inherit from several classes, given by arbitrary expressions. It receives their attributes, their methods (including their constructors) and their operators, which it can redefine:

```javascript
class Living {
    var life = 100;
    function Living() { this.life = 100; }
    function describe() { return "living"; }
}

class Person : Living, Human {
    var name;
    function Person(name) {
        // Call to the inherited class's constructor
        this.Living();
        this.name = name;
    }
    // Override
    function describe() { return this.name + " (" + this.life + ")"; }
}
```

## Static members

Preceded by the `static` keyword, attributes and methods belong to the class rather than to its instances:

```javascript
class Counter {
    static var total = 0;

    function Counter() {
        // Inside the class, "Counter." can be omitted
        total++;
    }

    static function create() {
        return new Counter();
    }
}

var c = Counter.create();
Counter.total;      // 1
```

A class can declare other classes, which are automatically static:

```javascript
class Game {
    class Player {
        var score = 0;
    }
}

var p = new Game.Player();
```

## Overloaded operators

The `operator` keyword followed by an operator's [overload name](operators.md) defines its behavior for the class's instances:

```javascript
class Point {
    var x = 0, y = 0;

    function Point(x, y) {
        if (x != undefined) this.x = x;
        if (y != undefined) this.y = y;
    }

    operator add(p) {
        return new Point(this.x + p.x, this.y + p.y);
    }

    operator equal(p) {
        return this.x == p.x && this.y == p.y;
    }

    // p[0] and p[1]
    operator get(i) {
        return i == 0 ? this.x : this.y;
    }

    // p(factor)
    operator call(factor) {
        return new Point(this.x * factor, this.y * factor);
    }
}

var c = new Point(3) + new Point(6, 4);     // Point(9, 4)
```

The overloaded operator is only called if the instance is the **first** operand: in `5 + p`, number addition applies (and gives `null`).

Without overloading, `==` and `!=` compare instance identity, and `=` replaces the variable's value.
