#include <cstring>
#include <iostream>
#include <fstream>

#include "../../src/litescript.hpp"

using namespace LiteScript;

/**
 * Read the content of a file
 * @param name The filename
 * @param i The line number reference
 * @return The content of the file
 */
std::string readfile(const char * name, unsigned int& i) {
    std::string s;
    std::ifstream file(name);
    if (!file.is_open())
        return std::string("error : can't open the file");

    char tmp[256];
    while (file.getline(tmp, 256)) {
        s += tmp;
        s += '\n';
        i++;
    }
    file.close();

    return s;
}

/**
 * The print callback to include in the state of script
 * @param s The assembly state
 * @param args The arguments
 * @return Undefined value
 */
Variable print_var(State& s, std::vector<Variable>& args) {
    std::cout << "print: ";
    for (unsigned int i = 0, sz = args.size(); i < sz; i++) {
        std::cout << args[i];
        if (i < sz - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
    return s.memory.Create(Type::UNDEFINED);
}

/**
 * The assembly instruction syntaxes
 */
std::array<const char *, 76> commands_instructions({
    "define.variable <name> : define a variable in the current namespace (1 pop)",
    "define.arg <index number> : define an argument (1 pop)",
    "define.return : define the return value (1 pop)",
    "",
    "value.pop : remove the heap last element (1 pop)",
    "value.copy : copy the heap last element (1 push)",
    "value.undefined : add an undefined value to the heap (1 push)",
    "value.null : add a null value to the heap (1 push)",
    "value.boolean <true|false> : add a boolean value to the heap (1 push)",
    "value.number <number> : add a number value to the heap (1 push)",
    "value.string <string> : add a string value to the heap (1 push)",
    "value.array : add an array value to the heap (1 push)",
    "value.class : add a class value to the heap (1 push)",
    "value.object : add a class object value to the heap, the class need to be on the top of the heap (1 pop, 1 push)",
    "value.callback <line number> : add a callback value to the heap with the line number of its (1 push)",
    "value.arg <index number> : add the nth argument value to the heap (1 push)",
    "value.args <index number> : add an array value to the heap that contain arguments from the referenced index to the end (1 push)",
    "value.variable <name> : add a variable to the heap (1 push)",
    "value.this : add the content of \"this\" value to the heap (1 push)",
    "",
    "push.nsp : push the namespace heap",
    "push.args : push the arguments heap",
    "pop.nsp : pop the namespace heap",
    "pop.args : pop the arguments heap",
    "",
    "return : return to the last callback",
    "",
    "operator.assign : apply assign operator (\"@a = @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.unary-plus : apply unary plus operator (\"+@a\") to the last heap value (1 pop, 1 push)",
    "operator.unary-minus : apply unary minus operator (\"-@a\") to the last heap value (1 pop, 1 push)",
    "operator.pre-incr : apply pre-increment operator (\"++@a\") to the last heap value (1 pop, 1 push)",
    "operator.pre-decr : apply pre-decrement operator (\"--@a\") to the last heap value (1 pop, 1 push)",
    "operator.post-incr : apply post-increment operator (\"@a++\") to the last heap value (1 pop, 1 push)",
    "operator.post-decr : apply post-decrement operator (\"@a--\") to the last heap value (1 pop, 1 push)",
    "operator.add : apply addition operator (\"@a + @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.sub : apply substraction operator (\"@a - @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.mul : apply multiplication operator (\"@a * @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.div : apply division operator (\"@a / @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.mod : apply modulo operator (\"@a % @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.add-assign : apply addition and assignation operator (\"@a += @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.sub-assign : apply substraction and assignation operator (\"@a -= @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.mul-assign : apply multiplication and assignation operator (\"@a *= @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.div-assign : apply division and assignation operator (\"@a /= @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.equ : apply equal comparison (\"@a == @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.dif : apply not equal comparison (\"@a != @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.great : apply greater comparison (\"@a > @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.less : apply lesser comparison (\"@a < @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.great-equ : apply greater or equal comparison (\"@a >= @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.less-equ : apply lesser or equal comparison (\"@a <= @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.log-not : apply logical not operation (\"!@a\") to the last heap value (1 pop, 1 push)",
    "operator.log-and : apply logical and operation (\"@a && @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.log-or : apply logical or operation (\"@a || @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.bit-not : apply bitwise not operation (\"~@a\") to the last heap value (1 pop, 1 push)",
    "operator.bit-and : apply bitwise and operation (\"@a & @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.bit-or : apply bitwise or operation (\"@a | @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.bit-xor : apply bitwise xor operation (\"@a ^ @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.lshift : apply left shift operation (\"@a << @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.rshift : apply right shift operation (\"@a >> @b\") to the 2 last heap values (2 pop, 1 push)",
    "operator.array : apply array access operation (\"@a[@b]\") to the 2 last heap values (2 pop, 1 push)",
    "operator.member <name> : apply member operation (\"@a.name\") to the last heap value (1 pop, 1 push)",
    "operator.call : apply calling operation (\"@a(arguments heap)\") to the last heap value (1 pop, 1 push)",
    "",
    "jump-to <line number> : jump to the indicated instruction line",
    "jump-if <line number> : jump to the indicated instruction line if the last heap value is true (1 pop)",
    "jump-else <line number> : jump to the indicated instruction line if the last heap value is false (1 pop)",
    "",
    "array.push-numeric <unsigned index> : add a value in the array (\"@a[index] <-- @b\") with a numeric index (1 pop)",
    "array.push-literal <string> : add a value in the array (\"@a[string] <-- @b\") with a string index (1 pop)",
    "",
    "class.push-static <name> : add a static member in the class (\"@a.name <-- @b\") (1 pop)",
    "class.push-unstatic <name> : add a not-static member in the class (\"@a.name <-- @b\") (1 pop)",
    "class.push-operator <index> : add an operator overloading in the class (\"@a.operator[index] <-- @b\") (1 pop)",
    "class.inherit: add a class inherit in the class (\"@a extends @b\"), the class to inherit need to be in the top of heap (1 pop)",
    "class.constructor <name> : define the constructor member in the class by its name",
    "",
    "namespace <name[.name]*> : define the current namespace base, \"global\" for return to the global namespace"
});

/**
 * The commands
 */
std::array<const char *, 5> command_help({
    "exit : stop execution",
    "file <filepath> : execute assembly code in a file",
    "help : redisplay this text",
    "instructions : display all instructions commands",
    "enter white line to execute previous code"
});

/**
 * For display instructions and commands
 * @param commnds_c The number of strings
 * @param commnds_v The strings
 */
void display_commands(int commnds_c, const char ** commnds_v) {
    for (unsigned int i = 0; i < commnds_c; i++) {
        if (strlen(commnds_v[i]) != 0)
            std::cout << "- " << commnds_v[i];
        std::cout << std::endl;
    }
}

/**
 * The main fonction
 */
int main(int argc, char * argv[]) {
    // We create the main memory and the script state
    Memory memory;
    State state(memory);
    // We create an assembly object
    Assembly assembly(state);

    // We add the "print" callback in the global namespace
    DeclareVariable(state, "print", CreateVariable(memory, print_var));

    // If there is a parameter, it's a file assembly script to execute
    if (argc > 1) {
        unsigned int i;
        // We read the file content
        std::string code = readfile(argv[1], i);
        if (code == "error : can't open the file") {
            std::cout << code << " \"" << argv[1] << "\"" << std::endl;
            return -1;
        }

        // We execute the assembly script
        Variable result = assembly.ExecuteAfter(code.c_str());
        // If there is an error, we print it
        if (assembly.error != Assembly::ErrorType::ASSM_ERROR_NO) {
            std::cout << "ERROR(" << assembly.line_error + 1 << "): " << assembly.GetError() << std::endl;
            return 1;
        }
        // Otherwise we print the result of the script
        // (it's the last value on the heap)
        else
            std::cout << (std::string)(result) << std::endl;
        return 0;
    }

    // Else we read lines on the keyboard
    // (the script execution is relatively the same as above)
    char cmd[256];
    std::string code;
    unsigned int line_num = 1, prev_num = 1;
    std::cout << "Enter assembly code :" << std::endl;
    display_commands(command_help.size(), command_help.data());
    while (true) {
        std::cout << std::endl;
        code.clear();
        while (true) {
            std::cout << line_num << " > ";
            std::cin.getline(cmd, 256);
            if (strncmp(cmd, "file ", 5) == 0) {
                code += readfile(cmd + 5, line_num);
                break;
            }
            else if (strncmp(cmd, "help", 4) == 0) {
                std::cout << "Commands :" << std::endl;
                display_commands(command_help.size(), command_help.data());
                std::cout << std::endl;
                continue;
            }
            else if (strncmp(cmd, "instructions", 12) == 0) {
                std::cout << "I"
                        "nstructions :" << std::endl;
                display_commands(commands_instructions.size(), commands_instructions.data());
                std::cout << std::endl;
                continue;
            }
            else if (strcmp(cmd, "exit") == 0)
                return 0;
            else if (cmd[0] == '\0')
                break;
            code += cmd;
            code += '\n';
            line_num++;
        }
        code.pop_back();

        Variable result = assembly.ExecuteAfter(code.c_str());
        if (assembly.error != Assembly::ErrorType::ASSM_ERROR_NO) {
            std::cout << "ERROR(" << assembly.line_error + prev_num << "): " << assembly.GetError() << std::endl;
            line_num = prev_num;
        }
        else {
            std::cout << (std::string)(result) << std::endl;
            prev_num = line_num;
        }
    }

}