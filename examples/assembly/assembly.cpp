#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include <litescript/litescript.hpp>

using namespace LiteScript;

/**
 * Read the content of a file
 * @param name The filename
 * @return The content of the file or nullopt if the file can't be opened
 */
std::optional<std::string> ReadFile(const std::string& name) {
    std::ifstream file(name, std::ios::binary);
    if (!file.is_open())
        return std::nullopt;
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

/**
 * The print function to include in the state of script
 * @param state The assembly state
 * @param args The arguments
 * @return Undefined value
 */
Variable Print(State& state, std::vector<Variable>& args) {
    std::cout << "print: ";
    for (std::size_t i = 0; i < args.size(); i++) {
        std::cout << args[i];
        if (i + 1 < args.size())
            std::cout << ", ";
    }
    std::cout << std::endl;
    return state.GetMemory().CreateUndefined();
}

/**
 * The assembly instruction syntaxes
 */
constexpr const char * INSTRUCTIONS_HELP[] = {
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
    "value.callback <line number> : add a callback value to the heap with the line number of its first instruction (1 push)",
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
};

/**
 * The commands
 */
constexpr const char * COMMANDS_HELP[] = {
    "exit : stop execution",
    "file <filepath> : execute assembly code in a file",
    "help : redisplay this text",
    "instructions : display all instructions commands",
    "enter white line to execute previous code"
};

/**
 * For display instructions and commands
 */
template <std::size_t N>
void DisplayCommands(const char * const (&commands)[N]) {
    for (const char * command : commands) {
        if (command[0] != '\0')
            std::cout << "- " << command;
        std::cout << std::endl;
    }
}

/**
 * The main function
 */
int main(int argc, char * argv[]) {
    // We create the main memory and the script state
    Memory memory;
    State state(memory);

    // We add the "print" function in the global namespace
    state.DeclareVariable("print", memory.RegisterFunction("print", Print));

    // If there is a parameter, it's a file assembly script to execute
    if (argc > 1) {
        auto code = ReadFile(argv[1]);
        if (!code) {
            std::cout << "error : can't open the file \"" << argv[1] << "\"" << std::endl;
            return -1;
        }
        // We execute the assembly script
        Assembly::Result result = Assembly::ExecuteAfter(state, *code);
        // If there is an error, we print it
        if (!result) {
            std::cout << "ERROR(" << result.line + 1 << "): " << result.GetMessage() << std::endl;
            return 1;
        }
        // Otherwise we print the result of the script (it's the last value on the stack)
        std::cout << result.value << std::endl;
        return 0;
    }

    // Else we read lines on the keyboard
    // (the instructions are added at the end of the same list, line numbers continue)
    std::size_t line_num = 1;
    std::cout << "Enter assembly code :" << std::endl;
    DisplayCommands(COMMANDS_HELP);
    while (true) {
        std::cout << std::endl;
        std::size_t first_line = line_num;
        std::string code, line;
        while (true) {
            std::cout << line_num << " > ";
            if (!std::getline(std::cin, line) || line == "exit")
                return 0;
            if (line.rfind("file ", 0) == 0) {
                auto content = ReadFile(line.substr(5));
                if (!content) {
                    std::cout << "error : can't open the file" << std::endl;
                    continue;
                }
                for (char c : *content)
                    line_num += c == '\n';
                code += *content;
                break;
            }
            if (line == "help") {
                std::cout << "Commands :" << std::endl;
                DisplayCommands(COMMANDS_HELP);
                std::cout << std::endl;
                continue;
            }
            if (line == "instructions") {
                std::cout << "Instructions :" << std::endl;
                DisplayCommands(INSTRUCTIONS_HELP);
                std::cout << std::endl;
                continue;
            }
            if (line.empty())
                break;
            code += line + '\n';
            line_num++;
        }

        Assembly::Result result = Assembly::ExecuteAfter(state, code);
        if (!result) {
            std::cout << "ERROR(" << result.line + first_line << "): " << result.GetMessage() << std::endl;
            line_num = first_line;
        }
        else {
            std::cout << result.value << std::endl;
        }
    }
}
