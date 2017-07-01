# LiteScript - A little script engine

LiteScript is a little script engine designed for developpers who want to include a script language in their application. The script language is similar to Javascript with object-oriented (it's planned to also create a language similar to assembler). It's developed in C++11 and doesn't need to be installed, it's enough to move the source files in the project.
(Development is still in progress)

The objective is to create an alternative of current script engines and integrating features that aren't present (like a state saving to stop execution and store it in a file, or an execution function instruction by instruction, etc...).

## Currently finished

* class **Object** (contain the variable block memory)
* class **Variable** (reference an **Object** in the memory)
* class **Memory** (contain all **Object**s)
* class **Type** (*abstract* : define all operations for every variable type)

*Types currently defined :*

* **undefined**
* **null**
* **boolean**
* **number**
* **string** (and **character** but it's hidden)
* **callback** (need to complete le calling operator)
* **object**
* **namespace**

## In future

* class/structure **Instruction** (a reduced instruction)
* class **State** (contain the script/assembler state)
* Garbage collector *mark and sweep* (**Variable**s contain also a reference counter)
* Assembler script language
* Javascript like object-oriented language

*Types to define :*

* **class**

## Example of Javascript like script

This example is the objective for the javascript like object-oriented language.

```javascript
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
        
    	this.name;
        this.age;
        this.gender;
    }
    
    // Destructor ?
    
    // Methods
    function Birthday() {
    	this.age++;
    }
    
    function Say(args...) {
    	Console.WriteLine(this.name + "say : ");
        Console.Write("> ");
    	foreach (var e in args) {
        	Console.Write(e + " ");
        }
        Console.WriteLine();
    }
    
    static function Marry(p1, p2) {
    	return new Person(Random.Name, 0, Random.Gender);
    }
}
```

My personnal website : [leptitdev.com](http://leptitdev.com) (french website).