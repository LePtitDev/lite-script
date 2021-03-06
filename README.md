# LiteScript - A little script engine

LiteScript is a little script engine designed for developpers who want to include a script language in their application. The script language is similar to Javascript with object-oriented (it's planned to also create a language similar to assembler). It's developed in C++11 and doesn't need to be installed, it's enough to move the source files in the project.
(Development is still in progress)

The objective is to create an alternative of current script engines and integrating features that aren't present (like a state saving to stop execution and store it in a file, or an execution function instruction by instruction, etc...).

The documentation is available [here](http://www.leptitdev.com/docs/litescript).

## Currently finished

* class **Object** (contain the variable block memory)
* class **Variable** (reference an **Object** in the memory)
* class **Memory** (contain all **Object**s)
* class **Type** (*abstract* : define all operations for every variable type)
* class/structure **Instruction** (a reduced instruction)
* class **State** (contain the script/assembler state)
* Assembler script language
* Javascript like object-oriented language
* Garbage collector *mark and sweep* (**Variable**s contain also a reference counter)
* Instructions, memory and state save/load
* Simple interface API
* French documentation

*Types currently defined :*

* **undefined**
* **null**
* **boolean**
* **number**
* **string** (and **character** but it's hidden)
* **callback** (need to complete le calling operator)
* **array**
* **namespace**
* **class** (inherit, static elements, constructor and operator overloading)
* **class object** (object created by a **class**)

## In future

* Documentation in english (coming soon)

## Example of Javascript like script

This example is the objective for the javascript like object-oriented language.

```javascript
// Enter in the namespace named "Game"
namespace Game;

// Person inherits Alive and Human
class Person : Alive, Human {

    // Attributes
    var name;
    var age;
    var gender;
    
    // Constructor
    function Person(name, age, gender) {
    	// Parent constructor
    	this.Alive();
        
    	this.name = name;
        this.age = age;
        this.gender = gender;
    }
    
    // Methods
    function Birthday() {
    	this.age++;
    }
    
    function Say(args...) {
    	Console.WriteLine(this.name + "say : ");
        Console.Write("> ");
    	for (var i = 0; i < args.length; i++) {
        	Console.Write(args[i] + " ");
        }
        Console.WriteLine();
    }
    
    // Operator overloading
    operator add(p) {
        return Person.Merry(this, p);
    }
    
    // Static method
    static function Marry(p1, p2) {
    	return new Person(Random.Name, 0, Random.Gender);
    }
}

// Return to the original namespace
namespace global;

var Anne = new Game.Person("Anne", 21, Gender.FEMALE);
var Henri = new Game.Person("Henri", 22, Gender.MALE);

print(Anne + Henri);
```
