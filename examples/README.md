# Examples

This examples can help to understand how to use this library.
A documentation will be created later.

You can to try to understand this library by reading headers of
source code if are motivated, there is some comments.
I advise you to read main classes in this order :
* `memory/type.hpp` : Describe the general class of a type
* `memory/object.hpp` : This is the content that a variable reference and are contained in the memory
* `memory/memory.hpp` : The main memory
* `memory/variable.hpp` : It's a reference to an object
* `script/state.hpp` : Contain the state of a script
* `script/assembly.hpp` : Execute assembly script instructions

### 1. Assembly

This is a program that can :
* execute an assembly script file
* read assembly script instructions on the keyboard

##### To execute an assembly script file :

`./executable "<filepath>"`

* executable : it's the built executable (you need to build it)
* filepath : it's the access file path that contain the assembly script code

##### To enter instructions on keyboard :

`./executable`

On execution :

* `exit` : stop execution
* `file <filepath>` : execute assembly code in a file
* `help` : diplay commands
* `instructions` : display instructions
* enter white line to execute previous code